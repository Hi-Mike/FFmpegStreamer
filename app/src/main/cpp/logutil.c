//
// Created by HiMike on 2017/1/5.
//

#include <string.h>
#include <libavutil/avassert.h>
#include <libavutil/bprint.h>
#include <time.h>
#include <libavutil/opt.h>
#include "logutil.h"

static FILE *report_file;
static int report_file_level = AV_LOG_DEBUG;
int hide_banner = 0;

static void log_callback_report(void *ptr, int level, const char *fmt, va_list vl) {
    va_list vl2;
    char line[1024];
    static int print_prefix = 1;

    va_copy(vl2, vl);
    av_log_default_callback(ptr, level, fmt, vl);
    av_log_format_line(ptr, level, fmt, vl2, line, sizeof(line), &print_prefix);
    va_end(vl2);
    if (report_file_level >= level) {
        fputs(line, report_file);
        fflush(report_file);
    }
}

static const OptionDef *find_option(const OptionDef *po, const char *name) {
    const char *p = strchr(name, ':');
    int len = p ? p - name : strlen(name);

    while (po->name) {
        if (!strncmp(name, po->name, len) && strlen(po->name) == len)
            break;
        po++;
    }
    return po;
}

int locate_option(int argc, char **argv, const OptionDef *options, const char *optname) {
    const OptionDef *po;
    int i;

    for (i = 1; i < argc; i++) {
        const char *cur_opt = argv[i];

        if (*cur_opt++ != '-')
            continue;

        po = find_option(options, cur_opt);
        if (!po->name && cur_opt[0] == 'n' && cur_opt[1] == 'o')
            po = find_option(options, cur_opt + 2);

        if ((!po->name && !strcmp(cur_opt, optname)) || (po->name && !strcmp(optname, po->name)))
            return i;

        if (!po->name || po->flags & HAS_ARG)
            i++;
    }
    return 0;
}

int opt_loglevel(void *optctx, const char *opt, const char *arg) {
    const struct {
        const char *name;
        int level;
    } log_levels[] = {
            {"quiet",   AV_LOG_QUIET},
            {"panic",   AV_LOG_PANIC},
            {"fatal",   AV_LOG_FATAL},
            {"error",   AV_LOG_ERROR},
            {"warning", AV_LOG_WARNING},
            {"info",    AV_LOG_INFO},
            {"verbose", AV_LOG_VERBOSE},
            {"debug",   AV_LOG_DEBUG},
            {"trace",   AV_LOG_TRACE},
    };
    char *tail;
    int level;
    int flags;
    int i;

    flags = av_log_get_flags();
    tail = strstr(arg, "repeat");
    if (tail)
        flags &= ~AV_LOG_SKIP_REPEATED;
    else
        flags |= AV_LOG_SKIP_REPEATED;

    av_log_set_flags(flags);
    if (tail == arg)
        arg += 6 + (arg[6] == '+');
    if (tail && !*arg)
        return 0;

    for (i = 0; i < FF_ARRAY_ELEMS(log_levels); i++) {
        if (!strcmp(log_levels[i].name, arg)) {
            av_log_set_level(log_levels[i].level);
            return 0;
        }
    }

    level = strtol(arg, &tail, 10);
    if (*tail) {
        av_log(NULL, AV_LOG_FATAL, "Invalid loglevel \"%s\". "
                "Possible levels are numbers or:\n", arg);
        for (i = 0; i < FF_ARRAY_ELEMS(log_levels); i++)
            av_log(NULL, AV_LOG_FATAL, "\"%s\"\n", log_levels[i].name);
//        exit_program(1);
    }
    av_log_set_level(level);
    return 0;
}

static void expand_filename_template(AVBPrint *bp, const char *templat, struct tm *tm) {
    int c;

    while ((
            c = *(templat++))) {
        if (c == '%') {
            if (!(c = *(templat++)))
                break;
            switch (c) {
                case 'p':
                    av_bprintf(bp, "%s", program_name);
                    break;
                case 't':
                    av_bprintf(bp, "%04d%02d%02d-%02d%02d%02d",
                               tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                               tm->tm_hour, tm->tm_min, tm->tm_sec);
                    break;
                case '%':
                    av_bprint_chars(bp, c, 1);
                    break;
            }
        } else {
            av_bprint_chars(bp, c, 1);
        }
    }
}

static int init_report(const char *env) {
    char *filename_template = NULL;
    char *key, *val;
    int ret, count = 0;
    time_t now;
    struct tm *tm;
    AVBPrint filename;

    if (report_file) /* already opened */
        return 0;
    time(&now);
    tm = localtime(&now);

    while (env && *env) {
        if ((ret = av_opt_get_key_value(&env, "=", ":", 0, &key, &val)) < 0) {
            if (count)
                av_log(NULL, AV_LOG_ERROR,
                       "Failed to parse FFREPORT environment variable: %s\n",
                       av_err2str(ret));
            break;
        }
        if (*env)
            env++;
        count++;
        if (!strcmp(key, "file")) {
            av_free(filename_template);
            filename_template = val;
            val = NULL;
        } else if (!strcmp(key, "level")) {
            char *tail;
            report_file_level = strtol(val, &tail, 10);
            if (*tail) {
                av_log(NULL, AV_LOG_FATAL, "Invalid report file level\n");
//                exit_program(1);
            }
        } else {
            av_log(NULL, AV_LOG_ERROR, "Unknown key '%s' in FFREPORT\n", key);
        }
        av_free(val);
        av_free(key);
    }

    av_bprint_init(&filename, 0, 1);
    expand_filename_template(&filename, av_x_if_null(filename_template, "%p-%t.log"), tm);
    av_free(filename_template);
    if (!av_bprint_is_complete(&filename)) {
        av_log(NULL, AV_LOG_ERROR, "Out of memory building report file name\n");
        return AVERROR(ENOMEM);
    }

    report_file = fopen(filename.str, "w");
    if (!report_file) {
        int ret = AVERROR(errno);
        av_log(NULL, AV_LOG_ERROR, "Failed to open report \"%s\": %s\n",
               filename.str, strerror(errno));
        return ret;
    }
    av_log_set_callback(log_callback_report);
    av_log(NULL, AV_LOG_INFO,
           "%s started on %04d-%02d-%02d at %02d:%02d:%02d\n""Report written to \"%s\"\n",
           program_name,
           tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
           tm->tm_hour, tm->tm_min, tm->tm_sec,
           filename.str);
    av_bprint_finalize(&filename, NULL);
    return 0;
}

static void dump_argument(const char *a) {
    const unsigned char *p;

    for (p = a; *p; p++)
        if (!((*p >= '+' && *p <= ':') || (*p >= '@' && *p <= 'Z') ||
              *p == '_' || (*p >= 'a' && *p <= 'z')))
            break;
    if (!*p) {
        fputs(a, report_file);
        return;
    }
    fputc('"', report_file);
    for (p = a; *p; p++) {
        if (*p == '\\' || *p == '"' || *p == '$' || *p == '`')
            fprintf(report_file, "\\%c", *p);
        else if (*p < ' ' || *p > '~')
            fprintf(report_file, "\\x%02x", *p);
        else
            fputc(*p, report_file);
    }
    fputc('"', report_file);
}

static void check_options(const OptionDef *po) {
    while (po->name) {
        if (po->flags & OPT_PERFILE)
            av_assert0(po->flags & (OPT_INPUT | OPT_OUTPUT));
        po++;
    }
}

void parse_loglevel(int argc, char **argv, const OptionDef *options) {
    int idx = locate_option(argc, argv, options, "loglevel");
    const char *env;

    check_options(options);

    if (!idx)
        idx = locate_option(argc, argv, options, "v");
    if (idx && argv[idx + 1])
        opt_loglevel(NULL, "loglevel", argv[idx + 1]);
    idx = locate_option(argc, argv, options, "report");
    if ((env = getenv("FFREPORT")) || idx) {
        init_report(env);
        if (report_file) {
            int i;
            fprintf(report_file, "Command line:\n");
            for (i = 0; i < argc; i++) {
                dump_argument(argv[i]);
                fputc(i < argc - 1 ? ' ' : '\n', report_file);
            }
            fflush(report_file);
        }
    }
    idx = locate_option(argc, argv, options, "hide_banner");
    if (idx)
        hide_banner = 1;
}