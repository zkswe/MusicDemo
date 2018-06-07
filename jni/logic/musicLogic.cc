#pragma once
/*
*此文件由GUI工具生成
*文件功能：用于处理用户的逻辑相应代码
*功能说明：
*========================onButtonClick_XXXX
当页面中的按键按下后系统会调用对应的函数，XXX代表GUI工具里面的[ID值]名称，
如Button1,当返回值为false的时候系统将不再处理这个按键，返回true的时候系统将会继续处理此按键。比如SYS_BACK.
*========================onSlideWindowItemClick_XXXX(int index) 
当页面中存在滑动窗口并且用户点击了滑动窗口的图标后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如slideWindow1;index 代表按下图标的偏移值
*========================onSeekBarChange_XXXX(int progress) 
当页面中存在滑动条并且用户改变了进度后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如SeekBar1;progress 代表当前的进度值
*========================ogetListItemCount_XXXX() 
当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表的总数目,XXX代表GUI工具里面的[ID值]名称，
如List1;返回值为当前列表的总条数
*========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表当前条目下的内容信息,XXX代表GUI工具里面的[ID值]名称，
如List1;pListItem 是贴图中的单条目对象，index是列表总目的偏移量。具体见函数说明
*========================常用接口===============
*LOGD(...)  打印调试信息的接口
*mTextXXX->setText("****") 在控件TextXXX上显示文字****
*mButton1->setSelected(true); 将控件mButton1设置为选中模式，图片会切换成选中图片，按钮文字会切换为选中后的颜色
*mSeekBar->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1->refreshListView() 让mListView1 重新刷新，当列表数据变化后调用
*mDashbroadView1->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度
*
* 在Eclipse编辑器中  使用 “alt + /”  快捷键可以打开智能提示
*/

#include "music/MusicManager.h"
#include "music/ID3Manager.h"
#include "utils/BitmapHelper.h"

static void updateControlInfo() {
	ID3MANAGER->ParseID3(MUSICMANAGER->getCurPlayMusic());
	const char *info = ID3MANAGER->GetTitle();
	if (!info || !info[0]) {
		info = "未知歌曲";
	}
	mTEXT_ID3_TITLEPtr->setText(info);

	info = ID3MANAGER->GetArtist();
	if (!info || !info[0]) {
		info = "未知歌手";
	}
	mTEXT_ID3_ARTISTPtr->setText(info);

	info = ID3MANAGER->GetAlbum();
	if (!info || !info[0]) {
		info = "未知专辑";
	}
	mTEXT_ID3_ALBUMPtr->setText(info);

	S_PIC_INFO pi = { NULL, 0 };
	ID3MANAGER->ParseID3Pic(MUSICMANAGER->getCurPlayMusic(), pi);
	PBITMAP pBmp = NULL;
	BitmapHelper::loadBitmapFromMem(pBmp, pi.pData, pi.dataLen, "jpg");
	delete[] pi.pData;

	mTEXT_ID3_PICPtr->setBackgroundBmp(pBmp);
	mTEXT_ID3_PICPtr->setText((pBmp == NULL) ? 'i' : '0');

	int pos = 0, dur = 0;
	if (MUSICMANAGER->getCurPlayMusic()) {
		dur = MUSICMANAGER->getDuration() / 1000;
		pos = MUSICMANAGER->getCurrentPosition() / 1000;
	}
	mSeekbar_play_progressPtr->setMax(dur);
	mSeekbar_play_progressPtr->setProgress(pos);

	char timeStr[10];
	sprintf(timeStr, "%02d:%02d", dur / 60, dur % 60);
	mTEXT_DURATIONPtr->setText(timeStr);

	sprintf(timeStr, "%02d:%02d", pos / 60, pos % 60);
	mTEXT_CUR_POSPtr->setText("00:00");

	mButton_playPtr->setSelected(MUSICMANAGER->isPlaying());

	int index = MUSICMANAGER->getCurPlayMusicIndex();
	if (index != -1) {
		index -= 2;
		if (index < 0) {
			index = 0;
		} else if (index > MUSICMANAGER->getMusicInfoCount() - 5) {
			index = MUSICMANAGER->getMusicInfoCount() - 5;
		}
		mListview_filenamePtr->setSelection(index);
	}
	mListview_filenamePtr->refreshListView();
}


class MusicMessageListener : public MusicManager::IMusicMessageListener {
public:
	virtual void onMusicMessage(EMessageType msg) {
		switch (msg) {
		case E_MSGTYPE_MUSIC_MOUNT_SDCARD:
			LOGD("onMusicMessage E_MSGTYPE_MUSIC_MOUNT_SDCARD...\n");
			break;

		case E_MSGTYPE_MUSIC_REMOVE_SDCARD:
			LOGD("onMusicMessage E_MSGTYPE_MUSIC_REMOVE_SDCARD...\n");
			MUSICMANAGER->stop();
			updateControlInfo();
			break;

		case E_MSGTYPE_MUSIC_INFO_UPDATE_BEGIN:
			LOGD("onMusicMessage E_MSGTYPE_MUSIC_INFO_UPDATE_BEGIN...\n");
			break;

		case E_MSGTYPE_MUSIC_INFO_UPDATE_END:
			LOGD("onMusicMessage E_MSGTYPE_MUSIC_INFO_UPDATE_END...\n");
			mListview_filenamePtr->refreshListView();
			break;

		case E_MSGTYPE_MUSIC_PLAY_STARTED: {
			LOGD("onMusicMessage E_MSGTYPE_MUSIC_PLAY_STARTED...\n");
			updateControlInfo();
			break;
		}

		case E_MSGTYPE_MUSIC_PLAY_COMPLETED:
		case E_MSGTYPE_MUSIC_PLAY_ERROR:
			mButton_playPtr->setSelected(false);
			MUSICMANAGER->next();
			break;
		}
	}
};

static MusicMessageListener sMusicMessageListener;
static bool sIsTracking = false;
static int sTrackProgress = -1;

/**
 * 注册定时器
 * 在此数组中添加即可
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	{100, 1000},
};

static void onUI_init() {
    //Tips :添加 UI初始化的显示代码到这里,如:mText1->setText("123");
	MUSICMANAGER->addMusicMessageListener(&sMusicMessageListener);
}

static void onUI_quit() {
	MUSICMANAGER->removeMusicMessageListener(&sMusicMessageListener);
}

static void onProtocolDataUpdate(const SProtocolData &data) {
    // 串口数据回调接口
}

static bool onUI_Timer(int id) {
    //Tips:添加定时器响应的代码到这里,但是需要在本文件的 REGISTER_ACTIVITY_TIMER_TAB 数组中 注册
    //id 是定时器设置时候的标签,这里不要写耗时的操作，否则影响UI刷新,ruturn:[true] 继续运行定时器;[false] 停止运行当前定时器
	if (id == 100) {
		if (MUSICMANAGER->getCurPlayMusic() && MUSICMANAGER->isPlaying()) {
			char timeStr[10];
			const int curPos = MUSICMANAGER->getCurrentPosition() / 1000;
			sprintf(timeStr, "%02d:%02d", curPos / 60, curPos % 60);
			mTEXT_CUR_POSPtr->setText(timeStr);

			if (!sIsTracking) {
				mSeekbar_play_progressPtr->setProgress(curPos);
			}
		}
	}

    return true;
}

static bool onmusicActivityTouchEvent(const MotionEvent &ev) {
    // 返回false触摸事件将继续传递到控件上，返回true表示该触摸事件在此被拦截了，不再传递到控件上
    return false;
}

static int getListItemCount_Listview_filename(const ZKListView *pListView) {
    return MUSICMANAGER->getMusicInfoCount();
}

static void obtainListItemData_Listview_filename(ZKListView *pListView, ZKListView::ZKListItem *pListItem, int index) {
    //LOGD(" obtainListItemData_ Listview_filename  !!!\n");
	SMediaInfo mi;
	MUSICMANAGER->getMusicInfo(index, mi);
	pListItem->setText(mi.mediaFileName);

	ZKListView::ZKListSubItem *pSub = pListItem->findSubItemByID(ID_MUSIC_Select_SubItem);
	if (pSub) {
		pSub->setVisible(index == MUSICMANAGER->getCurPlayMusicIndex());
	}
}

static void onListItemClick_Listview_filename(ZKListView *pListView, int index, int id) {
    //LOGD(" onListItemClick_ Listview_filename  !!!\n");
	SMediaInfo mi;
	MUSICMANAGER->getMusicInfo(index, mi);
	MUSICMANAGER->play(mi.mediaPath.c_str());
}

static bool onButtonClick_Button_play(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_play !!!\n");
	if (MUSICMANAGER->getCurPlayMusic()) {
		if (MUSICMANAGER->isPlaying()) {
			MUSICMANAGER->pause();
			pButton->setSelected(false);
		} else {
			MUSICMANAGER->resume();
			pButton->setSelected(true);
		}
	}

    return true;
}

static bool onButtonClick_Button_prev(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_prev !!!\n");
	MUSICMANAGER->prev();
    return true;
}

static bool onButtonClick_Button_next(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_next !!!\n");
	MUSICMANAGER->next();
    return true;
}

static bool onButtonClick_Button_open_wifi(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_open_wifi !!!\n");
	EASYUICONTEXT->openActivity("NetSettingActivity");
    return true;
}

static void onProgressChanged_Seekbar_play_progress(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged Seekbar_play_progress %d !!!\n", progress);
	sTrackProgress = progress;
}

static void onProgressStartTrackingTouch_play_progress(ZKSeekBar *pSeekBar) {
	sIsTracking = true;
}

static void onProgressStopTrackingTouch_play_progress(ZKSeekBar *pSeekBar) {
	sIsTracking = false;
	if (sTrackProgress >= 0) {
		MUSICMANAGER->seekTo(sTrackProgress * 1000);
		sTrackProgress = -1;
	}
}
static bool onButtonClick_Button_voice(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_voice !!!\n");
    return true;
}

