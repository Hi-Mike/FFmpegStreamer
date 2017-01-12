//
// Created by HiMike on 2017/1/5.
//
#include <jni.h>

#ifndef FFMPEGSTREAMER_TEST_H
#define FFMPEGSTREAMER_TEST_H

JNIEXPORT jint JNICALL Java_cn_mike_ffmpegstreamer_FFmpegUtil_ffmpeg(JNIEnv *env, jclass type,
                                                                     jint argc,
                                                                     jobjectArray cmdlines);

#endif //FFMPEGSTREAMER_TEST_H
