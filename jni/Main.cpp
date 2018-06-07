#include "entry/EasyUIContext.h"
#include "utils/Log.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

void onEasyUIInit(EasyUIContext *pContext) {

}

void onEasyUIDeinit(EasyUIContext *pContext) {

}

const char* onStartupApp(EasyUIContext *pContext) {
	return "musicActivity";
}

#ifdef __cplusplus
}
#endif  /* __cplusplus */

