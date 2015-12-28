#ifndef _DEBUG_H
#define _DEBUG_H


#define USING_DEBUG 1
//#define PLATFORM_NDK		//compile on ndk
//#define PLATFORM_LINUX		//compile on linux
#define PLATFORM_ANDROID	//compile on android	
#define LOG_TAG "audio"


#ifdef 	PLATFORM_NDK
#include <android/log.h>
#endif

#ifdef	PLATFORM_ANDROID
#include <cutils/log.h>
#endif


		
//=================================================================
//using debug
#ifdef  USING_DEBUG

//==========================================
//compile on ndk
#ifdef 	PLATFORM_NDK		


#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__)
#define D(...)	  __android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG, __VA_ARGS__)

#endif //PLATFORM_NDK
//==========================================
//compile on linux
#ifdef PLATFORM_LINUX		
#define D printf
#define LOGE printf
#endif //PLATFORM_LINUX
//==========================================
//compile on android
#ifdef PLATFORM_ANDROID
#define D LOGD
#endif

//=================================================================
//not using debug
#else 
//==========================================
//compile on ndk
#ifdef 	PLATFORM_NDK
#define D(...) do{}while(0)
#define LOGE(...) do{}while(0)
#endif //PLATFORM_NDK
//==========================================
//compile on linux
#ifdef PLATFORM_LINUX	
#define D(...) do{}while(0)
#define LOGE(...) do{}while(0)
#endif //PLATFORM_LINUX

//==========================================
//compile on android
#ifdef PLATFORM_ANDROID
#define D(...) do{}while(0)
#define LOGD(...) do{}while(0)
#define LOGE(...) do{}while(0)

#endif


#endif //USING_DEBUG
//=================================================================


void debug_buffer(const char *prefix,unsigned char *buffer,int len);

void debug_info(const char *prefix,unsigned char *buffer,int len);


#endif

