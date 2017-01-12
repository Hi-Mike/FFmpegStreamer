//
// Created by HiMike on 2017/1/5.
//

#ifndef FFMPEGSTREAMER_UTILS_H
#define FFMPEGSTREAMER_UTILS_H

#include <stdint.h>

extern const char program_name[];

typedef struct OptionDef {
    const char *name;
    int flags;
#define HAS_ARG    0x0001
#define OPT_BOOL   0x0002
#define OPT_EXPERT 0x0004
#define OPT_STRING 0x0008
#define OPT_VIDEO  0x0010
#define OPT_AUDIO  0x0020
#define OPT_INT    0x0080
#define OPT_FLOAT  0x0100
#define OPT_SUBTITLE 0x0200
#define OPT_INT64  0x0400
#define OPT_EXIT   0x0800
#define OPT_DATA   0x1000
#define OPT_PERFILE  0x2000     /* the option is per-file (currently ffmpeg-only).
                                   implied by OPT_OFFSET or OPT_SPEC */
#define OPT_OFFSET 0x4000       /* option is specified as an offset in a passed optctx */
#define OPT_SPEC   0x8000       /* option is to be stored in an array of SpecifierOpt.
                                   Implies OPT_OFFSET. Next element after the offset is
                                   an int containing element count in the array. */
#define OPT_TIME  0x10000
#define OPT_DOUBLE 0x20000
#define OPT_INPUT  0x40000
#define OPT_OUTPUT 0x80000

    union {
        void *dst_ptr;

        int (*func_arg)(void *, const char *, const char *);

        size_t off;
    } u;

    const char *help;
    const char *argname;
} OptionDef;

int locate_option(int argc, char **argv, const OptionDef *options, const char *optname);

int opt_loglevel(void *optctx, const char *opt, const char *arg);

void parse_loglevel(int argc, char **argv, const OptionDef *options);

#endif //FFMPEGSTREAMER_UTILS_H
