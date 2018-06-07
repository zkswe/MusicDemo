/*
 * MediaInfo.h
 *
 *  Created on: Jan 24, 2018
 *      Author: guoxs
 */

#ifndef _MUSIC_MEDIAINFO_H_
#define _MUSIC_MEDIAINFO_H_

#include <string>

typedef enum {
	E_MEDIA_TYPE_AUDIO,
	E_MEDIA_TYPE_VIDEO,
	E_MEDIA_TYPE_IMAGE,
	E_MEDIA_TYPE_UNKNOWN
} EMediaType;

typedef struct {
	EMediaType mediaType;
	std::string mediaPath;
	std::string mediaFileName;
} SMediaInfo;

#endif /* _MUSIC_MEDIAINFO_H_ */
