/*
 * MusicManager.h
 *
 *  Created on: Jan 24, 2018
 *      Author: guoxs
 */

#ifndef _MUSIC_MUSICMANAGER_H_
#define _MUSIC_MUSICMANAGER_H_

#include "MediaScanner.h"
#include "os/MountMonitor.h"
#include "media/ZKMediaPlayer.h"

typedef enum {
	E_MSGTYPE_MUSIC_MOUNT_SDCARD,
	E_MSGTYPE_MUSIC_REMOVE_SDCARD,
	E_MSGTYPE_MUSIC_INFO_UPDATE_BEGIN,
	E_MSGTYPE_MUSIC_INFO_UPDATE_END,
	E_MSGTYPE_MUSIC_PLAY_STARTED,
	E_MSGTYPE_MUSIC_PLAY_COMPLETED,
	E_MSGTYPE_MUSIC_PLAY_ERROR
} EMessageType;

class MusicManager {
public:
	~MusicManager();

	static MusicManager* getInstance();

	void getMusicInfoList(std::vector<SMediaInfo> &musicInfoList);
	void getMusicInfo(int index, SMediaInfo &si);

	int getMusicInfoCount() const;

	bool isScanning() const;

	void play(const char *pFilePath, int msec = 0);
	void pause();
	void resume();
	void seekTo(int msec);
	void stop();

	bool isPlaying();

	void setVolume(float volume);

	int getDuration();
	int getCurrentPosition();

	void prev();
	void next();

	int getCurPlayMusicIndex() const;
	const char* getCurPlayMusic() const;

	class IMusicMessageListener {
	public:
		virtual ~IMusicMessageListener() { }
		virtual void onMusicMessage(EMessageType msg) = 0;
	};

	void addMusicMessageListener(IMusicMessageListener *pListener);
	void removeMusicMessageListener(IMusicMessageListener *pListener);

private:
	MusicManager();

	void notifyMusicMessage(EMessageType msg);

	class MountListener : public MountMonitor::IMountListener {
	public:
		MountListener(MusicManager &mm) : mMM(mm) { }

		virtual void notify(int what, int status, const char *msg);

	private:
		MusicManager &mMM;
	};

	class MusicScanListener : public MediaScanner::IScanListener {
	public:
		MusicScanListener(MusicManager &mm) : mMM(mm) { }

		virtual void onScanBegin();
		virtual void onScanEnd();
		virtual void onScanMediaInfo(const SMediaInfo &mi);

	private:
		MusicManager &mMM;
	};

	class PlayerMessageListener : public ZKMediaPlayer::IPlayerMessageListener {
	public:
		PlayerMessageListener(MusicManager &mm) : mMM(mm) { }
		virtual void onPlayerMessage(ZKMediaPlayer *pMediaPlayer, int msg, void *pMsgData);

	private:
		MusicManager &mMM;
	};

private:
	MountListener mMountListener;
	MusicScanListener mMusicScanListener;

	std::vector<IMusicMessageListener *> mMusicMessageListenerList;
	mutable Mutex mListenerLock;

	std::vector<SMediaInfo> mMusicInfoList;
	mutable Mutex mLock;

	ZKMediaPlayer *mMediaPlayerPtr;
	PlayerMessageListener mPlayerMessageListener;

	bool mIsScanning;

	int mCurPlayIndex;
};

#define MUSICMANAGER		MusicManager::getInstance()

#endif /* _MUSIC_MUSICMANAGER_H_ */
