cmake_minimum_required(VERSION 3.4.1)
set(lib_src_DIR ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})
include_directories(
        ${CMAKE_SOURCE_DIR}/include
)

add_library(avcodec-57_lib SHARED IMPORTED)
set_target_properties(avcodec-57_lib PROPERTIES IMPORTED_LOCATION
                             ${lib_src_DIR}/libavcodec-57.so)

add_library(avformat-57_lib SHARED IMPORTED)
set_target_properties(avformat-57_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavformat-57.so)

add_library(avutil-55_lib SHARED IMPORTED)
set_target_properties(avutil-55_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavutil-55.so)

add_library(swresample-2_lib SHARED IMPORTED)
set_target_properties(swresample-2_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libswresample-2.so)

add_library(swscale-4_lib SHARED IMPORTED)
set_target_properties(swscale-4_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libswscale-4.so)

add_library(filter-6_lib SHARED IMPORTED)
set_target_properties(filter-6_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavfilter-6.so)

add_library(device-57_lib SHARED IMPORTED)
set_target_properties(device-57_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavdevice-57.so)

add_library(avresample-3_lib SHARED IMPORTED)
set_target_properties(avresample-3_lib PROPERTIES IMPORTED_LOCATION
                        ${lib_src_DIR}/libavresample-3.so)

# build application's shared lib
add_library(ffmpeg SHARED
            native-lib.c)

# Include libraries needed for VideoPlayer lib
target_link_libraries(ffmpeg
log
android
avcodec-57_lib
avformat-57_lib
avutil-55_lib
swresample-2_lib
swscale-4_lib
filter-6_lib
device-57_lib
avresample-3_lib)