package cn.mike.ffmpegstreamer;

import java.io.File;

/**
 * Created by HiMike on 2017/1/5.
 */

public class FFmpegUtil {
    static {
//        System.loadLibrary("avutil");
//        System.loadLibrary("swresample");
//        System.loadLibrary("avcodec");
//        System.loadLibrary("avformat");
//        System.loadLibrary("swscale");
//        System.loadLibrary("avfilter");
//        System.loadLibrary("avdevice");
        System.loadLibrary("ffmpeg");
    }

    public static native int ffmpeg(int argc, String[] cmdlines);

    public static void append(File src, File append, File out) {
//        String fmt = "ffmpeg -i concat:%s|%s -acodec copy %s";
        String fmt = "%s";
//        fmt = String.format(fmt, src.getAbsolutePath(), append.getAbsolutePath(), out.getAbsolutePath());
        fmt = String.format(fmt, src.getAbsolutePath());
        String[] cmds = fmt.split(" ");
        int argc = cmds.length;
        ffmpeg(argc, cmds);
    }
}