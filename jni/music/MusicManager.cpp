/*
 * MusicManager.cpp
 *
 *  Created on: Jan 24, 2018
 *      Author: guoxs
 */

#include "MusicManager.h"
#include "utils/Log.h"

MusicManager::MusicManager() :
	mMountListener(*this),
	mMusicScanListener(*this),
	mPlayerMessageListener(*this),
	mIsScanning(false),
	mCurPlayIndex(-1) {
	mMediaPlayerPtr = new ZKMediaPlayer(ZKMediaPlayer::E_MEDIA_TYPE_AUDIO);
	mMediaPlayerPtr->setPlayerMessageListener(&mPlayerMessageListener);

	MOUNTMONITOR->addMountListener(&mMountListener);
	MEDIASCANNER->addScanListener(&mMusicScanListener);

	if (MOUNTMONITOR->isMount()) {
		MEDIASCANNER->startScanDir(MOUNTMONITOR->getSDcardPath());
	}
}

MusicManager::~MusicManager() {
	MOUNTMONITOR->removeMountListener(&mMountListener);
	MEDIASCANNER->removeScanListener(&mMusicScanListener);

	delete mMediaPlayerPtr;
}

MusicManager* MusicManager::getInstance() {
	static MusicManager sMM;
	return &sMM;
}

void MusicManager::getMusicInfoList(std::vector<SMediaInfo> &musicInfoList) {
	Mutex::Autolock _l(mLock);
	musicInfoList.assign(mMusicInfoList.begin(), mMusicInfoList.end());
}

void MusicManager::getMusicInfo(int index, SMediaInfo &si) {
	Mutex::Autolock _l(mLock);
	if (!mIsScanning && ((index >= 0) || (index < mMusicInfoList.size()))) {
		si = mMusicInfoList[index];
	}
}

int MusicManager::getMusicInfoCount() const {
	Mutex::Autolock _l(mLock);
	return !mIsScanning ? mMusicInfoList.size() : 0;
}

bool MusicManager::isScanning() const {
	return mIsScanning;
}

void MusicManager::play(const char *pFilePath, int msec) {
	Mutex::Autolock _l(mLock);
	int i = 0;
	for (; i < mMusicInfoList.size(); ++i) {
		if (mMusicInfoList[i].mediaPath == pFilePath) {
			mCurPlayIndex = i;
			break;
		}
	}

	if (i == mMusicInfoList.size()) {
		mCurPlayIndex = -1;
	}

	mMediaPlayerPtr->play(pFilePath, msec);
}

void MusicManager::pause() {
	mMediaPlayerPtr->pause();
}

void MusicManager::resume() {
	mMediaPlayerPtr->resume();
}

void MusicManager::seekTo(int msec) {
	mMediaPlayerPtr->seekTo(msec);
}

void MusicManager::stop() {
	mMediaPlayerPtr->stop();
	mCurPlayIndex = -1;
}

bool MusicManager::isPlaying() {
	return mMediaPlayerPtr->isPlaying();
}

void MusicManager::setVolume(float volume) {
	mMediaPlayerPtr->setVolume(volume, volume);
}

int MusicManager::getDuration() {
	return mMediaPlayerPtr->getDuration();
}

int MusicManager::getCurrentPosition() {
	return mMediaPlayerPtr->getCurrentPosition();
}

void MusicManager::prev() {
	Mutex::Autolock _l(mLock);
	if (!mMusicInfoList.empty()) {
		if (mCurPlayIndex == -1) {
			mCurPlayIndex = 0;
		} else {
			mCurPlayIndex = (mCurPlayIndex - 1 + mMusicInfoList.size()) % mMusicInfoList.size();
		}

		mMediaPlayerPtr->play(mMusicInfoList[mCurPlayIndex].mediaPath.c_str());
	}
}

void MusicManager::next() {
	Mutex::Autolock _l(mLock);
	if (!mMusicInfoList.empty()) {
		if (mCurPlayIndex == -1) {
			mCurPlayIndex = 0;
		} else {
			mCurPlayIndex = (mCurPlayIndex + 1 + mMusicInfoList.size()) % mMusicInfoList.size();
		}

		mMediaPlayerPtr->play(mMusicInfoList[mCurPlayIndex].mediaPath.c_str());
	}
}

int MusicManager::getCurPlayMusicIndex() const {
	Mutex::Autolock _l(mLock);
	return mCurPlayIndex;
}

const char* MusicManager::getCurPlayMusic() const {
	Mutex::Autolock _l(mLock);
	if ((mCurPlayIndex != -1) && (mCurPlayIndex < mMusicInfoList.size())) {
		return mMusicInfoList[mCurPlayIndex].mediaPath.c_str();
	}

	return NULL;
}

void MusicManager::addMusicMessageListener(IMusicMessageListener *pListener) {
	Mutex::Autolock _l(mListenerLock);
	if (pListener) {
		mMusicMessageListenerList.push_back(pListener);
	}
}

void MusicManager::removeMusicMessageListener(IMusicMessageListener *pListener) {
	Mutex::Autolock _l(mListenerLock);
	for (int i = 0; i < mMusicMessageListenerList.size(); i++) {
		if (mMusicMessageListenerList[i] == pListener) {
			mMusicMessageListenerList.erase(mMusicMessageListenerList.begin() + i);
			return;
		}
	}
}

void MusicManager::notifyMusicMessage(EMessageType msg) {
	Mutex::Autolock _l(mListenerLock);
	for (int i = 0; i < mMusicMessageListenerList.size(); i++) {
		mMusicMessageListenerList[i]->onMusicMessage(msg);
	}
}

void MusicManager::MountListener::notify(int what, int status, const char *msg) {
	switch (status) {
	case MountMonitor::E_MOUNT_STATUS_MOUNTED:
		mMM.notifyMusicMessage(E_MSGTYPE_MUSIC_MOUNT_SDCARD);
		MEDIASCANNER->startScanDir(msg);
		break;

	case MountMonitor::E_MOUNT_STATUS_REMOVE:
		MEDIASCANNER->stopScanDir();

		{
			Mutex::Autolock _l(mMM.mLock);
			mMM.mMusicInfoList.clear();
		}

		mMM.notifyMusicMessage(E_MSGTYPE_MUSIC_REMOVE_SDCARD);
		break;
	}
}


void MusicManager::MusicScanListener::onScanBegin() {
	mMM.mIsScanning = true;
	mMM.notifyMusicMessage(E_MSGTYPE_MUSIC_INFO_UPDATE_BEGIN);
}

void MusicManager::MusicScanListener::onScanEnd() {
	mMM.mIsScanning = false;
	mMM.notifyMusicMessage(E_MSGTYPE_MUSIC_INFO_UPDATE_END);
}

void MusicManager::MusicScanListener::onScanMediaInfo(const SMediaInfo &mi) {
	Mutex::Autolock _l(mMM.mLock);
	if (mi.mediaType == E_MEDIA_TYPE_AUDIO) {
		mMM.mMusicInfoList.push_back(mi);
	}
}

void MusicManager::PlayerMessageListener::onPlayerMessage(ZKMediaPlayer *pMediaPlayer, int msg, void *pMsgData) {
	switch (msg) {
	case ZKMediaPlayer::E_MSGTYPE_ERROR_INVALID_FILEPATH:
	case ZKMediaPlayer::E_MSGTYPE_ERROR_MEDIA_ERROR:
		mMM.notifyMusicMessage(E_MSGTYPE_MUSIC_PLAY_ERROR);
		break;

	case ZKMediaPlayer::E_MSGTYPE_PLAY_STARTED:
		mMM.notifyMusicMessage(E_MSGTYPE_MUSIC_PLAY_STARTED);
		break;

	case ZKMediaPlayer::E_MSGTYPE_PLAY_COMPLETED:
		mMM.notifyMusicMessage(E_MSGTYPE_MUSIC_PLAY_COMPLETED);
		break;
	}
}
