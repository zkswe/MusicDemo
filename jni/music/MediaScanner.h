/*
 * MediaScanner.h
 *
 *  Created on: Jan 24, 2018
 *      Author: guoxs
 */

#ifndef _MUSIC_MEDIA_SCANNER_H_
#define _MUSIC_MEDIA_SCANNER_H_

#include <vector>

#include "MediaInfo.h"
#include "system/Thread.h"

class MediaScanner {
public:
	static MediaScanner* getInstance();

	void startScanDir(const char *pDir);
	void stopScanDir();

	class IScanListener {
	public:
		virtual ~IScanListener() { }
		virtual void onScanBegin() = 0;
		virtual void onScanEnd() = 0;
		virtual void onScanMediaInfo(const SMediaInfo &mi) = 0;
	};

	void addScanListener(IScanListener *pListener);
	void removeScanListener(IScanListener *pListener);

private:
	MediaScanner();

	void notifyScanBegin();
	void notifyScanEnd();
	void notifyScanMediaInfo(const SMediaInfo &mi);

	void doScanDir(const char *pDir);

	class ScanThread : public Thread {
	public:
		ScanThread(MediaScanner &ms) : mMS(ms) { }
		virtual bool threadLoop();

		std::string mScanDir;

	private:
		MediaScanner &mMS;
	};

private:
	std::vector<IScanListener *> mScanListenerList;
	mutable Mutex mListenerLock;

	ScanThread mScanThread;
	bool mReqExitScan;
};

#define MEDIASCANNER		MediaScanner::getInstance()

#endif /* _MUSIC_MEDIA_SCANNER_H_ */
