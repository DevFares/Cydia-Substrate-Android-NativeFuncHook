//
// Created by F8LEFT on 2016/2/29.
//

#include "hello.h"
#include "substrate.h"

#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>

#define LOGOPEN

#ifdef LOG_TAG
#undef LOG_TAG
#endif


#ifdef LOGOPEN
#define LOG_TAG "F8LEFT"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#else
#define LOGE(...)
#define LOGD(...)
#define LOGW(...)
#define LOGI(...)
#endif

#define NULL 0

int loadbase = 0;

__attribute__((constructor))
void fixBase() {
    loadbase = 50;
}

__attribute__ ((destructor))
void unfixBase() {
    loadbase = 1000;
}

int enc1(int a) {
    return a / 2;
}

int enc2(int a) {
    return enc1(a) * 23333;
}

//public native String Hello3(MainActivity ma);
jint Hello3(JNIEnv *env, jobject obj, jint base) {
    int sum = 0;
    for (int i = loadbase; i < base; i++) {
        sum = sum + i;
    }
    return enc2(sum);
}


jint HelloHook(JNIEnv *env, jobject obj, jint base) {
    int sum = 0;
    for (int i = 0; i < base; i++) {
        sum = sum + i;
    }
    return sum;
}


MSInitialize{
    MSHookFunction((void*)Hello3, (void*)HelloHook, NULL);
}

static int registerNativeMethods(JNIEnv *env, const char *className,
                                 JNINativeMethod *gMethods, int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static const char *gClassName = "f8/hellonative/MainActivity";
static JNINativeMethod gMethods[] = {
        {"Hello3", "(I)I", (void *) Hello3},
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    if (registerNativeMethods(env, gClassName, gMethods,
                              sizeof(gMethods) / sizeof(gMethods[0])) == JNI_FALSE) {
        return -1;
    }

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    return;
}