/*
 * MediaScanner.cpp
 *
 *  Created on: Jan 24, 2018
 *      Author: guoxs
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "MediaScanner.h"
#include "utils/Log.h"

// 文件路径最大长度
#define MAX_PATH	260

/**
 * 是否是不扫描的文件夹
 */
static bool isIgnoredFolder(const char *pFolder) {
	// .开头的文件夹为隐藏文件夹，不扫描
	if (pFolder[0] == '.') {
		return true;
	}

	// 不扫描的文件夹
	static const char *sIgnoredFolderTab[] = {
		"LOST.DIR", "$RECYCLE.BIN", "cacheData", "cache", "adcache",
		"VideoCache", "Lyric", "log", "IssueReporter",
		"autonavi", "autonavidata", "NaviOne", "IGO"
	};

	static const int N = sizeof(sIgnoredFolderTab) / sizeof(sIgnoredFolderTab[0]);
	for (int i = 0; i < N; ++i) {
		if (strcasecmp(pFolder, sIgnoredFolderTab[i]) == 0) {
			return true;
		}
	}

	return false;
}

/**
 * 获取文件后缀名
 */
static const char* getFileExtName(const char *pFile) {
	// 查找'.'最后出现的位置
	const char *p = strrchr(pFile, '.');
	return (p != NULL) ? p + 1 : NULL;
}

/**
 * 匹配后缀名
 */
static bool isMatchExtName(const char **pExtNameTab, int lenOfTab, const char *pExtName) {
	for (int i = 0; i < lenOfTab; ++i) {
		if (strcasecmp(pExtName, pExtNameTab[i]) == 0) {
			return true;
		}
	}

	return false;
}

static EMediaType getMediaType(const char *pFile) {
	if (pFile == NULL) {
		return E_MEDIA_TYPE_UNKNOWN;
	}

	const char *pExtName = getFileExtName(pFile);
	if (pExtName != NULL) {
		static const char *sAudioExtNameTab[] = {
			"mp3", "wma", "wav", "aac", "ma4", "flac",
	        "ape", "ogg", "ra", "m4a", "m4r", "mp2"
		};
		static const int AUDIO_TAB_LEN = sizeof(sAudioExtNameTab) / sizeof(sAudioExtNameTab[0]);

		static const char *sVideoExtNameTab[] = {
			"mp4", "avi", "wmv", "flv", "ts", "m2ts",
			"tp", "mov", "vob", "mkv", "3gp", "3ga",
			"asf", "divx", "mpg", "mpeg", "m4v", "m2v"
		};
		static const int VIDEO_TAB_LEN = sizeof(sVideoExtNameTab) / sizeof(sVideoExtNameTab[0]);

		static const char *sImageExtNameTab[] = {
			"png", "jpg", "jpeg", "bmp", "gif"
		};
		static const int IMAGE_TAB_LEN = sizeof(sImageExtNameTab) / sizeof(sImageExtNameTab[0]);

		if (isMatchExtName(sAudioExtNameTab, AUDIO_TAB_LEN, pExtName)) {
			return E_MEDIA_TYPE_AUDIO;
		}

		if (isMatchExtName(sVideoExtNameTab, VIDEO_TAB_LEN, pExtName)) {
			return E_MEDIA_TYPE_VIDEO;
		}

		if (isMatchExtName(sImageExtNameTab, IMAGE_TAB_LEN, pExtName)) {
			return E_MEDIA_TYPE_IMAGE;
		}
	}

	return E_MEDIA_TYPE_UNKNOWN;
}

MediaScanner::MediaScanner() :
	mScanThread(*this),
	mReqExitScan(false) {

}

MediaScanner* MediaScanner::getInstance() {
	static MediaScanner sMS;
	return &sMS;
}

void MediaScanner::startScanDir(const char *pDir) {
	if (pDir == NULL) {
		return;
	}

	stopScanDir();

	LOGD("startScanDir pDir: %s\n", pDir);

	mReqExitScan = false;
	mScanThread.mScanDir = pDir;
	mScanThread.run();
}

void MediaScanner::stopScanDir() {
	if (mScanThread.isRunning()) {
		LOGD("stopScanDir ...\n");
		mReqExitScan = true;
		mScanThread.requestExitAndWait();
	}
}

void MediaScanner::addScanListener(IScanListener *pListener) {
	Mutex::Autolock _l(mListenerLock);
	if (pListener) {
		mScanListenerList.push_back(pListener);
	}
}

void MediaScanner::removeScanListener(IScanListener *pListener) {
	Mutex::Autolock _l(mListenerLock);
	for (int i = 0; i < mScanListenerList.size(); i++) {
		if (mScanListenerList[i] == pListener) {
			mScanListenerList.erase(mScanListenerList.begin() + i);
			return;
		}
	}
}

void MediaScanner::notifyScanBegin() {
	Mutex::Autolock _l(mListenerLock);
	for (int i = 0; i < mScanListenerList.size(); i++) {
		mScanListenerList[i]->onScanBegin();
	}
}

void MediaScanner::notifyScanEnd() {
	Mutex::Autolock _l(mListenerLock);
	for (int i = 0; i < mScanListenerList.size(); i++) {
		mScanListenerList[i]->onScanEnd();
	}
}

void MediaScanner::notifyScanMediaInfo(const SMediaInfo &mi) {
	Mutex::Autolock _l(mListenerLock);
	for (int i = 0; i < mScanListenerList.size(); i++) {
		mScanListenerList[i]->onScanMediaInfo(mi);
	}
}

void MediaScanner::doScanDir(const char *pDir) {
	DIR *dp = opendir(pDir);

	if (dp) {
		struct dirent *entry;
		struct stat buff;

		SMediaInfo mi;

		char filePath[MAX_PATH];
		strcpy(filePath, pDir);
		int dirLen = strlen(pDir);

		filePath[dirLen++] = '/';
		filePath[dirLen] = '\0';

		// 指向开始改动的位置
		char *pFileName = filePath + dirLen;

		while (((entry = readdir(dp)) != NULL) && !mReqExitScan) {
			if ((dirLen + strlen(entry->d_name)) >= MAX_PATH) {
				// 路径长度超过260则不处理该路径的文件
				continue;
			}

			strcpy(pFileName, entry->d_name);
			if (stat(filePath, &buff)) {
				LOGE("doScanDir pFile: %s, stat error!", filePath);
				continue;
			}

			if (S_ISREG(buff.st_mode)) {	// 文件
				mi.mediaType = getMediaType(filePath);
				if (mi.mediaType != E_MEDIA_TYPE_UNKNOWN) {
					mi.mediaPath = filePath;
					mi.mediaFileName = pFileName;
					notifyScanMediaInfo(mi);
				}
			} else if (S_ISDIR(buff.st_mode)) {	// 目录
				if ((strcmp(".", entry->d_name) != 0) && (strcmp("..", entry->d_name) != 0)) {
					if (!isIgnoredFolder(entry->d_name)) {
						doScanDir(filePath);
					} else {
						LOGE("ignore path: %s !", filePath);
					}
				}
			}

			usleep(5000);
		}

		closedir(dp);
	}
}


bool MediaScanner::ScanThread::threadLoop() {
	mMS.notifyScanBegin();
	mMS.doScanDir(mScanDir.c_str());
	mMS.notifyScanEnd();

	return false;
}
