/***********************************************
/gen auto by zuitools
***********************************************/
#include "musicActivity.h"

/*TAG:GlobalVariable全局变量*/
static ZKSeekBar* mSoundSeekbarPtr;
static ZKWindow* mSoundWindowPtr;
static ZKButton* mButton_voicePtr;
static ZKTextView* mTEXT_ID3_PICPtr;
static ZKSeekBar* mSeekbar_play_progressPtr;
static ZKTextView* mTEXT_DURATIONPtr;
static ZKTextView* mTEXT_CUR_POSPtr;
static ZKButton* mButton_nextPtr;
static ZKButton* mButton_prevPtr;
static ZKButton* mButton_playPtr;
static ZKListView* mListview_filenamePtr;
static ZKTextView* mTEXT_ID3_ALBUMPtr;
static ZKTextView* mTEXT_ID3_ARTISTPtr;
static ZKTextView* mTEXT_ID3_TITLEPtr;
static ZKTextView* mTextview7Ptr;
static ZKTextView* mTextview4Ptr;
static ZKTextView* mTextview1Ptr;


/*register activity*/
REGISTER_ACTIVITY(musicActivity);

typedef struct {
	int id; // 定时器ID ， 不能重复
	int time; // 定时器  时间间隔  单位 毫秒
}S_ACTIVITY_TIMEER;

#include "logic/musicLogic.cc"

/***********/
typedef struct {
    int id;
    const char *pApp;
} SAppInfo;

/**
 *点击跳转window
 */
static SAppInfo sAppInfoTab[] = {
//  { ID_MAIN_TEXT, "TextViewActivity" },
};

/***************/
typedef bool (*ButtonCallback)(ZKButton *pButton);
/**
 * button onclick表
 */
typedef struct {
    int id;
    ButtonCallback callback;
}S_ButtonCallback;

/*TAG:ButtonCallbackTab按键映射表*/
static S_ButtonCallback sButtonCallbackTab[] = {
    ID_MUSIC_Button_voice, onButtonClick_Button_voice,
    ID_MUSIC_Button_next, onButtonClick_Button_next,
    ID_MUSIC_Button_prev, onButtonClick_Button_prev,
    ID_MUSIC_Button_play, onButtonClick_Button_play,
};
/***************/


typedef void (*SeekBarCallback)(ZKSeekBar *pSeekBar, int progress);
typedef void (*SeekBarStartTrackingTouchCallback)(ZKSeekBar *pSeekBar);
typedef void (*SeekBarStopTrackingTouchCallback)(ZKSeekBar *pSeekBar);
typedef struct {
    int id;
    SeekBarCallback callback;
	SeekBarStartTrackingTouchCallback startTrackingTouchCB;
    SeekBarStopTrackingTouchCallback stopTrackingTouchCB;
}S_ZKSeekBarCallback;
/*TAG:SeekBarCallbackTab*/
static S_ZKSeekBarCallback SZKSeekBarCallbackTab[] = {
    ID_MUSIC_SoundSeekbar, onProgressChanged_SoundSeekbar, onProgressStartTrackingTouch_SoundSeekbar, onProgressStopTrackingTouch_SoundSeekbar,
    ID_MUSIC_Seekbar_play_progress, onProgressChanged_Seekbar_play_progress, onProgressStartTrackingTouch_play_progress, onProgressStopTrackingTouch_play_progress,
};

typedef int (*ListViewGetItemCountCallback)(const ZKListView *pListView);
typedef void (*ListViewobtainListItemDataCallback)(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index);
typedef void (*ListViewonItemClickCallback)(ZKListView *pListView, int index, int id);
typedef struct {
    int id;
    ListViewGetItemCountCallback getListItemCountCallback;
    ListViewobtainListItemDataCallback obtainListItemDataCallback;
    ListViewonItemClickCallback onItemClickCallback;
}S_ListViewFunctionsCallback;
/*TAG:ListViewFunctionsCallback*/
static S_ListViewFunctionsCallback SListViewFunctionsCallbackTab[] = {
    ID_MUSIC_Listview_filename, getListItemCount_Listview_filename, obtainListItemData_Listview_filename, onListItemClick_Listview_filename,
};


typedef void (*SlideWindowItemClickCallback)(ZKSlideWindow *pSlideWindow, int index);
typedef struct {
    int id;
    SlideWindowItemClickCallback onSlideItemClickCallback;
}S_SlideWindowItemClickCallback;
/*TAG:SlideWindowFunctionsCallbackTab*/
static S_SlideWindowItemClickCallback SSlideWindowItemClickCallbackTab[] = {
};


typedef void (*EditTextInputCallback)(const std::string &text);
typedef struct {
    int id;
    EditTextInputCallback onEditTextChangedCallback;
}S_EditTextInputCallback;
/*TAG:EditTextInputCallback*/
static S_EditTextInputCallback SEditTextInputCallbackTab[] = {
};

typedef void (*VideoViewCallback)(ZKVideoView *pVideoView, int msg);
typedef struct {
    int id; //VideoView ID
    bool loop; // 是否是轮播类型
    int defaultvolume;//轮播类型时,默认视频音量
    VideoViewCallback onVideoViewCallback;
}S_VideoViewCallback;
/*TAG:VideoViewCallback*/
static S_VideoViewCallback SVideoViewCallbackTab[] = {
};


musicActivity::musicActivity() {
	//todo add init code here
	mVideoLoopIndex = 0;
	mVideoLoopErrorCount = 0;
}

musicActivity::~musicActivity() {
	//todo add init file here
    // 退出应用时需要反注册
    EASYUICONTEXT->unregisterGlobalTouchListener(this);
    onUI_quit();
    unregisterProtocolDataUpdateListener(onProtocolDataUpdate);
}

const char* musicActivity::getAppName() const{
	return "music.ftu";
}

//TAG:onCreate
void musicActivity::onCreate() {
	Activity::onCreate();
    mSoundSeekbarPtr = (ZKSeekBar*)findControlByID(ID_MUSIC_SoundSeekbar);if(mSoundSeekbarPtr!= NULL){mSoundSeekbarPtr->setSeekBarChangeListener(this);}
    mSoundWindowPtr = (ZKWindow*)findControlByID(ID_MUSIC_SoundWindow);
    mButton_voicePtr = (ZKButton*)findControlByID(ID_MUSIC_Button_voice);
    mTEXT_ID3_PICPtr = (ZKTextView*)findControlByID(ID_MUSIC_TEXT_ID3_PIC);
    mSeekbar_play_progressPtr = (ZKSeekBar*)findControlByID(ID_MUSIC_Seekbar_play_progress);if(mSeekbar_play_progressPtr!= NULL){mSeekbar_play_progressPtr->setSeekBarChangeListener(this);}
    mTEXT_DURATIONPtr = (ZKTextView*)findControlByID(ID_MUSIC_TEXT_DURATION);
    mTEXT_CUR_POSPtr = (ZKTextView*)findControlByID(ID_MUSIC_TEXT_CUR_POS);
    mButton_nextPtr = (ZKButton*)findControlByID(ID_MUSIC_Button_next);
    mButton_prevPtr = (ZKButton*)findControlByID(ID_MUSIC_Button_prev);
    mButton_playPtr = (ZKButton*)findControlByID(ID_MUSIC_Button_play);
    mListview_filenamePtr = (ZKListView*)findControlByID(ID_MUSIC_Listview_filename);if(mListview_filenamePtr!= NULL){mListview_filenamePtr->setListAdapter(this);mListview_filenamePtr->setItemClickListener(this);}
    mTEXT_ID3_ALBUMPtr = (ZKTextView*)findControlByID(ID_MUSIC_TEXT_ID3_ALBUM);
    mTEXT_ID3_ARTISTPtr = (ZKTextView*)findControlByID(ID_MUSIC_TEXT_ID3_ARTIST);
    mTEXT_ID3_TITLEPtr = (ZKTextView*)findControlByID(ID_MUSIC_TEXT_ID3_TITLE);
    mTextview7Ptr = (ZKTextView*)findControlByID(ID_MUSIC_Textview7);
    mTextview4Ptr = (ZKTextView*)findControlByID(ID_MUSIC_Textview4);
    mTextview1Ptr = (ZKTextView*)findControlByID(ID_MUSIC_Textview1);
	onUI_init();
        // 注册监听全局触摸
    EASYUICONTEXT->registerGlobalTouchListener(this);
    registerProtocolDataUpdateListener(onProtocolDataUpdate); 

    rigesterActivityTimer();
}

void musicActivity::onClick(ZKBase *pBase) {
	//TODO: add widget onClik code 
    int buttonTablen = sizeof(sButtonCallbackTab) / sizeof(S_ButtonCallback);
    for (int i = 0; i < buttonTablen; ++i) {
        if (sButtonCallbackTab[i].id == pBase->getID()) {
            sButtonCallbackTab[i].callback((ZKButton*)pBase);
            break;
        }
    }

    int len = sizeof(sAppInfoTab) / sizeof(sAppInfoTab[0]);
    for (int i = 0; i < len; ++i) {
        if (sAppInfoTab[i].id == pBase->getID()) {
            EASYUICONTEXT->openActivity(sAppInfoTab[i].pApp);
            return;
        }
    }

	Activity::onClick(pBase);
}

void musicActivity::onResume() {
	Activity::onResume();
	startVideoLoopPlayback();
}

void musicActivity::onPause() {
	Activity::onPause();
	stopVideoLoopPlayback();
}

void musicActivity::onIntent(const Intent *intentPtr) {
	//TODO:  
	Activity::onIntent(intentPtr);
}

bool musicActivity::onTimer(int id) {
	return onUI_Timer(id);
}

void musicActivity::onProgressChanged(ZKSeekBar *pSeekBar, int progress){

    int seekBarTablen = sizeof(SZKSeekBarCallbackTab) / sizeof(S_ZKSeekBarCallback);
    for (int i = 0; i < seekBarTablen; ++i) {
        if (SZKSeekBarCallbackTab[i].id == pSeekBar->getID()) {
            SZKSeekBarCallbackTab[i].callback(pSeekBar, progress);
            break;
        }
    }
}

void musicActivity::onStartTrackingTouch(ZKSeekBar *pSeekBar) {
	int seekBarTablen = sizeof(SZKSeekBarCallbackTab) / sizeof(S_ZKSeekBarCallback);
	for (int i = 0; i < seekBarTablen; ++i) {
		if (SZKSeekBarCallbackTab[i].id == pSeekBar->getID()) {
			SZKSeekBarCallbackTab[i].startTrackingTouchCB(pSeekBar);
			break;
		}
	}
}

void musicActivity::onStopTrackingTouch(ZKSeekBar *pSeekBar) {
	int seekBarTablen = sizeof(SZKSeekBarCallbackTab) / sizeof(S_ZKSeekBarCallback);
	for (int i = 0; i < seekBarTablen; ++i) {
		if (SZKSeekBarCallbackTab[i].id == pSeekBar->getID()) {
			SZKSeekBarCallbackTab[i].stopTrackingTouchCB(pSeekBar);
			break;
		}
	}
}

int musicActivity::getListItemCount(const ZKListView *pListView) const{
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            return SListViewFunctionsCallbackTab[i].getListItemCountCallback(pListView);
            break;
        }
    }
    return 0;
}

void musicActivity::obtainListItemData(ZKListView *pListView,ZKListView::ZKListItem *pListItem, int index){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].obtainListItemDataCallback(pListView, pListItem, index);
            break;
        }
    }
}

void musicActivity::onItemClick(ZKListView *pListView, int index, int id){
    int tablen = sizeof(SListViewFunctionsCallbackTab) / sizeof(S_ListViewFunctionsCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SListViewFunctionsCallbackTab[i].id == pListView->getID()) {
            SListViewFunctionsCallbackTab[i].onItemClickCallback(pListView, index, id);
            break;
        }
    }
}

void musicActivity::onSlideItemClick(ZKSlideWindow *pSlideWindow, int index) {
    int tablen = sizeof(SSlideWindowItemClickCallbackTab) / sizeof(S_SlideWindowItemClickCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SSlideWindowItemClickCallbackTab[i].id == pSlideWindow->getID()) {
            SSlideWindowItemClickCallbackTab[i].onSlideItemClickCallback(pSlideWindow, index);
            break;
        }
    }
}

bool musicActivity::onTouchEvent(const MotionEvent &ev) {
    return onmusicActivityTouchEvent(ev);
}

void musicActivity::onTextChanged(ZKTextView *pTextView, const std::string &text) {
    int tablen = sizeof(SEditTextInputCallbackTab) / sizeof(S_EditTextInputCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SEditTextInputCallbackTab[i].id == pTextView->getID()) {
            SEditTextInputCallbackTab[i].onEditTextChangedCallback(text);
            break;
        }
    }
}

void musicActivity::rigesterActivityTimer() {
    int tablen = sizeof(REGISTER_ACTIVITY_TIMER_TAB) / sizeof(S_ACTIVITY_TIMEER);
    for (int i = 0; i < tablen; ++i) {
        S_ACTIVITY_TIMEER temp = REGISTER_ACTIVITY_TIMER_TAB[i];
        registerTimer(temp.id, temp.time);
    }
}


void musicActivity::onVideoPlayerMessage(ZKVideoView *pVideoView, int msg) {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
        if (SVideoViewCallbackTab[i].id == pVideoView->getID()) {
        	if (SVideoViewCallbackTab[i].loop) {
                //循环播放
        		videoLoopPlayback(pVideoView, msg, i);
        	} else if (SVideoViewCallbackTab[i].onVideoViewCallback != NULL){
        	    SVideoViewCallbackTab[i].onVideoViewCallback(pVideoView, msg);
        	}
            break;
        }
    }
}

void musicActivity::videoLoopPlayback(ZKVideoView *pVideoView, int msg, int callbackTabIndex) {

	switch (msg) {
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED:
		LOGD("ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED\n");
		pVideoView->setVolume(SVideoViewCallbackTab[callbackTabIndex].defaultvolume / 10.0);
		mVideoLoopErrorCount = 0;
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_ERROR:
		/**错误处理 */
		++mVideoLoopErrorCount;
		if (mVideoLoopErrorCount > 100) {
			LOGD("video loop error counts > 100, quit loop playback !");
            break;
		} //不用break, 继续尝试播放下一个
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED:
		LOGD("ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED\n");
        std:vector<std::string> videolist;
        std::string fileName(getAppName());
        if (fileName.size() < 4) {
             LOGD("getAppName size < 4, ignore!");
             break;
        }
        fileName = fileName.substr(0, fileName.length() - 4) + "_video_list.txt";
        fileName = "/mnt/extsd/" + fileName;
        if (!parseVideoFileList(fileName.c_str(), videolist)) {
            LOGD("parseVideoFileList failed !");
		    break;
        }
		if (pVideoView && !videolist.empty()) {
			mVideoLoopIndex = (mVideoLoopIndex + 1) % videolist.size();
			pVideoView->play(videolist[mVideoLoopIndex].c_str());
		}
		break;
	}
}

void musicActivity::startVideoLoopPlayback() {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
    	if (SVideoViewCallbackTab[i].loop) {
    		ZKVideoView* videoView = (ZKVideoView*)findControlByID(SVideoViewCallbackTab[i].id);
    		if (!videoView) {
    			return;
    		}
    		//循环播放
    		videoLoopPlayback(videoView, ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED, i);
    		return;
    	}
    }
}

void musicActivity::stopVideoLoopPlayback() {
    int tablen = sizeof(SVideoViewCallbackTab) / sizeof(S_VideoViewCallback);
    for (int i = 0; i < tablen; ++i) {
    	if (SVideoViewCallbackTab[i].loop) {
    		ZKVideoView* videoView = (ZKVideoView*)findControlByID(SVideoViewCallbackTab[i].id);
    		if (!videoView) {
    			return;
    		}
    		if (videoView->isPlaying()) {
    		    videoView->stop();
    		}
    		return;
    	}
    }
}

bool musicActivity::parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList) {
	mediaFileList.clear();
	if (NULL == pFileListPath || 0 == strlen(pFileListPath)) {
        LOGD("video file list is null!");
		return false;
	}

	string tmp;
	ifstream is(pFileListPath, ios_base::in);
	if (!is.is_open()) {
		LOGD("cann't open file %s \n", pFileListPath);
		return false;
	}
	while (getline(is, tmp)) {
		removeCharFromString(tmp, '\"');
		removeCharFromString(tmp, '\r');
		removeCharFromString(tmp, '\n');
		if (tmp.size() > 1) {
     		mediaFileList.push_back(string(tmp.c_str()));
		}
	}
	LOGD("(f:%s, l:%d) parse fileList[%s], get [%d]files\n", __FUNCTION__,
			__LINE__, pFileListPath, mediaFileList.size());
	for (size_t i = 0; i < mediaFileList.size(); i++) {
		LOGD("file[%d]:[%s]\n", i, mediaFileList[i].c_str());
	}
	is.close();

	return true;
}

int musicActivity::removeCharFromString(string& nString, char c) {
    string::size_type   pos;
    while(1)
    {
        pos = nString.find(c);
        if(pos != string::npos)
        {
            nString.erase(pos, 1);
        }
        else
        {
            break;
        }
    }
    return (int)nString.size();
}
