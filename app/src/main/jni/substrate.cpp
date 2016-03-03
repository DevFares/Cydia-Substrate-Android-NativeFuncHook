//
// Created by F8LEFT on 2016/3/3.
//

#include "substrate.h"
#include <dlfcn.h>

typedef bool (FMSHookProcess)(pid_t pid, const char *library);
typedef MSImageRef (FMSGetImageByName)(const char *file);
typedef void *(FMSFindSymbol)(MSImageRef image, const char *name);
typedef void (FMSHookFunction)(void *symbol, void *replace, void **result);

MSImageRef imageBase = NULL;
FMSHookProcess *fmsHookProcess = NULL;
FMSGetImageByName *fmsGetImageByName = NULL;
FMSFindSymbol *fmsFindSymbol = NULL;
FMSHookFunction *fmsHookFunction = NULL;

#ifdef __ANDROID__
typedef void (FMSJavaHookClassLoad)(JNIEnv *jni, const char *name, void (*callback)(JNIEnv *, jclass, void *), void *data);
typedef void (FMSJavaHookMethod)(JNIEnv *jni, jclass _class, jmethodID methodID, void *function, void **result);
typedef void (FMSJavaBlessClassLoader)(JNIEnv *jni, jobject loader);

typedef MSJavaObjectKey (FMSJavaCreateObjectKey)();
typedef void (FMSJavaReleaseObjectKey)(MSJavaObjectKey key);
typedef void *(FMSJavaGetObjectKey)(JNIEnv *jni, jobject object, MSJavaObjectKey key);
typedef void (FMSJavaSetObjectKey)(JNIEnv *jni, jobject object, MSJavaObjectKey key, void *value, void (*clean)(void *, JNIEnv *, void *), void *data);

FMSJavaHookClassLoad *fmsJavaHookClassLoad = NULL;
FMSJavaHookMethod *fmsJavaHookMethod = NULL;
FMSJavaBlessClassLoader *fmsJavaBlessClassLoader = NULL;
FMSJavaCreateObjectKey *fmsJavaCreateObjectKey = NULL;
FMSJavaReleaseObjectKey *fmsJavaReleaseObjectKey = NULL;
FMSJavaGetObjectKey *fmsJavaGetObjectKey = NULL;
FMSJavaSetObjectKey *fmsJavaSetObjectKey = NULL;
#endif
//Initialise function
MSInitialize {
    bool isDalvik = dlopen("libdvm.so", RTLD_NOLOAD) != NULL;
    if (isDalvik)
        imageBase = dlopen("libsubstrate.so-dvm", RTLD_NOW);
    else
        imageBase = dlopen("libsubstrate.so", RTLD_NOW);
    fmsHookProcess = (FMSHookProcess*)dlsym((void*)imageBase, "MSHookProcess");
    fmsGetImageByName = (FMSGetImageByName*)dlsym((void*)imageBase, "MSGetImageByName");
    fmsFindSymbol = (FMSFindSymbol*)dlsym((void*)imageBase, "MSFindSymbol");
    fmsHookFunction = (FMSHookFunction*)dlsym((void*)imageBase, "MSHookFunction");

#ifdef __ANDROID__
    if (isDalvik) {
        fmsJavaHookClassLoad = (FMSJavaHookClassLoad*)dlsym((void*)imageBase, "MSJavaHookClassLoad");
        fmsJavaHookMethod = (FMSJavaHookMethod*)dlsym((void*)imageBase, "MSJavaHookMethod");
        fmsJavaBlessClassLoader = (FMSJavaBlessClassLoader *)dlsym((void*)imageBase, "MSJavaBlessClassLoader");
        fmsJavaCreateObjectKey = (FMSJavaCreateObjectKey *)dlsym((void*)imageBase, "MSJavaCreateObjectKey");
        fmsJavaReleaseObjectKey = (FMSJavaReleaseObjectKey *)dlsym((void*)imageBase, "MSJavaReleaseObjectKey");
        fmsJavaGetObjectKey = (FMSJavaGetObjectKey *)dlsym((void*)imageBase, "MSJavaGetObjectKey");
        fmsJavaSetObjectKey = (FMSJavaSetObjectKey *)dlsym((void*)imageBase, "MSJavaSetObjectKey");
    }
#endif
};

bool MSHookProcess(pid_t pid, const char *library) {
    if (fmsHookFunction != NULL)
        return fmsHookProcess(pid, library);
    return false;
}

MSImageRef MSGetImageByName(const char *file) {
    if (fmsGetImageByName != NULL)
        return fmsGetImageByName(file);
    return NULL;
}

void *MSFindSymbol(MSImageRef image, const char *name) {
    if (fmsFindSymbol != NULL)
        return fmsFindSymbol(image, name);
    return NULL;
}

void MSHookFunction(void *symbol, void *replace, void **result) {
    if (fmsHookFunction != NULL)
        return fmsHookFunction(symbol, replace, result);
    return;
}

#ifdef __ANDROID__
void MSJavaHookClassLoad(JNIEnv *jni, const char *name, void (*callback)(JNIEnv *, jclass, void *), void *data)
{
    if (fmsJavaHookClassLoad != NULL)
        return fmsJavaHookClassLoad(jni, name, callback, data);
    return;
}

void MSJavaHookMethod(JNIEnv *jni, jclass _class, jmethodID methodID, void *function, void **result)
{
    if (fmsJavaHookMethod != NULL)
        return fmsJavaHookMethod(jni, _class, methodID, function, result);
    return;
}

void MSJavaBlessClassLoader(JNIEnv *jni, jobject loader) {
    if (fmsJavaBlessClassLoader != NULL)
        return fmsJavaBlessClassLoader(jni, loader);
    return;
}

MSJavaObjectKey MSJavaCreateObjectKey() {
    if (fmsJavaCreateObjectKey != NULL)
        return fmsJavaCreateObjectKey();
    return NULL;
}

void MSJavaReleaseObjectKey(MSJavaObjectKey key) {
    if (fmsJavaReleaseObjectKey != NULL)
        return fmsJavaReleaseObjectKey(key);
    return;
}
void *MSJavaGetObjectKey(JNIEnv *jni, jobject object, MSJavaObjectKey key)
{
    if (fmsJavaGetObjectKey != NULL)
        return fmsJavaGetObjectKey(jni, object, key);
    return NULL;
}

void MSJavaSetObjectKey(JNIEnv *jni, jobject object, MSJavaObjectKey key, void *value, void (*clean)(void *, JNIEnv *, void *), void *data)
{
    if (fmsJavaSetObjectKey != NULL)
        return fmsJavaSetObjectKey(jni, object, key, value, clean, data);
    return;
}
#endif