/*
 * ID3Manager.h
 *
 *  Created on: 2016-1-6
 *      Author: guoxs
 */

#ifndef _ID3_MANAGER_H_
#define _ID3_MANAGER_H_

#include <jni.h>

#include "CommDef.h"

class CMP3ID3Parser;
class CWMAID3Parser;
class CM4AID3Parser;

class CID3Manager {
public:
	~CID3Manager();

	static CID3Manager* getInstance();

	bool ParseID3(const char *pPath);
	bool ParseID3Pic(const char *pPath, const char *pSavePicPath);
	bool ParseID3Pic(const char *pPath, S_PIC_INFO &picInfo);

	const char* GetTitle() const {
		return m_id3Info.title;
	}

	const char* GetArtist() const {
		return m_id3Info.artist;
	}

	const char* GetAlbum() const {
		return m_id3Info.album;
	}

private:
	CID3Manager();

private:
	S_ID3_INFO m_id3Info;
	
	CMP3ID3Parser *m_pMP3ID3Parser;
	CWMAID3Parser *m_pWMAID3Parser;
	CM4AID3Parser *m_pM4AID3Parser;
};

#define ID3MANAGER			CID3Manager::getInstance()

#endif /* _ID3_MANAGER_H_ */
