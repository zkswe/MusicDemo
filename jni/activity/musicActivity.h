/***********************************************
/gen auto by zuitools
***********************************************/
#ifndef __MUSICACTIVITY_H__
#define __MUSICACTIVITY_H__


#include "app/Activity.h"
#include "entry/EasyUIContext.h"

#include "uart/ProtocolData.h"
#include "uart/ProtocolParser.h"

#include "utils/Log.h"
#include "control/ZKDigitalClock.h"
#include "control/ZKButton.h"
#include "control/ZKCircleBar.h"
#include "control/ZKDiagram.h"
#include "control/ZKListView.h"
#include "control/ZKPointer.h"
#include "control/ZKQRCode.h"
#include "control/ZKTextView.h"
#include "control/ZKSeekBar.h"
#include "control/ZKEditText.h"
#include "control/ZKVideoView.h"
#include "window/ZKSlideWindow.h"

/*TAG:Macro宏ID*/
#define ID_MUSIC_Button_voice    20006
#define ID_MUSIC_SubItem1    20005
#define ID_MUSIC_TEXT_ID3_PIC    50009
#define ID_MUSIC_Seekbar_play_progress    91001
#define ID_MUSIC_TEXT_DURATION    50007
#define ID_MUSIC_TEXT_CUR_POS    50002
#define ID_MUSIC_Button_next    20003
#define ID_MUSIC_Button_prev    20002
#define ID_MUSIC_Button_play    20001
#define ID_MUSIC_Listview_filename    80001
#define ID_MUSIC_Select_SubItem    20007
#define ID_MUSIC_TEXT_ID3_ALBUM    50010
#define ID_MUSIC_TEXT_ID3_ARTIST    50008
#define ID_MUSIC_TEXT_ID3_TITLE    50006
#define ID_MUSIC_Textview7    50005
#define ID_MUSIC_Textview4    50003
#define ID_MUSIC_Textview1    50001
/*TAG:Macro宏ID END*/

class musicActivity : public Activity, 
                     public ZKSeekBar::ISeekBarChangeListener, 
                     public ZKListView::IItemClickListener,
                     public ZKListView::AbsListAdapter,
                     public ZKSlideWindow::ISlideItemClickListener,
                     public EasyUIContext::ITouchListener,
                     public ZKEditText::ITextChangeListener,
                     public ZKVideoView::IVideoPlayerMessageListener
{
public:
    musicActivity();
    virtual ~musicActivity();

protected:
    /*TAG:PROTECTED_FUNCTION*/
    virtual const char* getAppName() const;
    virtual void onCreate();
    virtual void onClick(ZKBase *pBase);
    virtual void onResume();
    virtual void onPause();
    virtual void onIntent(const Intent *intentPtr);
    virtual bool onTimer(int id);

    virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress);
	virtual void onStartTrackingTouch(ZKSeekBar *pSeekBar);
    virtual void onStopTrackingTouch(ZKSeekBar *pSeekBar);

    virtual int getListItemCount(const ZKListView *pListView) const;
    virtual void obtainListItemData(ZKListView *pListView, ZKListView::ZKListItem *pListItem, int index);
    virtual void onItemClick(ZKListView *pListView, int index, int subItemIndex);

    virtual void onSlideItemClick(ZKSlideWindow *pSlideWindow, int index);

    virtual bool onTouchEvent(const MotionEvent &ev);

    virtual void onTextChanged(ZKTextView *pTextView, const string &text);

    void rigesterActivityTimer();

    virtual void onVideoPlayerMessage(ZKVideoView *pVideoView, int msg);
    void videoLoopPlayback(ZKVideoView *pVideoView, int msg, int callbackTabIndex);
    void startVideoLoopPlayback();
    void stopVideoLoopPlayback();
    bool parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList);
    int removeCharFromString(string& nString, char c);


private:
    /*TAG:PRIVATE_VARIABLE*/
    int mVideoLoopIndex;
    int mVideoLoopErrorCount;

};

#endif
