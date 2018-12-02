#!/usr/bin/bash

# Learning Resources: 
# https://wiki.libsdl.org/Android
# https://hg.libsdl.org/SDL/file/default/docs/README-android.md

# Java package name: "org.libdearimgui.android"
# cpp switches: -std=c++11 -fexceptions -Wall -Wformat

# TODO list:
# Download Android SDK (26+) and export path so we can use 'tools', 'platform-tools'
# Download Android NDK (r15c+) and export path so we can use 'ndk-build'

DIR_NAME="android_sdl2_example"
SDL2_LATEST='https://www.libsdl.org/release/SDL2-2.0.9.tar.gz'

# Example commands for setting up 'Dear ImGui' Android project
wget -O "SDL2.tar.gz" $SDL2_LATEST && tar xzf SDL2.tar.gz
mkdir $DIR_NAME
cp -r ./SDL2/android-project/* ./$DIR_NAME
cd $DIR_NAME/app/jni/src
ln -s ../../../SDL2 ../SDL

# Copy or Link Dear ImGui library into src directory

# Copy necessary impl files that get imgui to work

# Edit Application.mk so that c++_shared is being used, add exceptions if necessary

# Edit the Android.mk files so that the CPP/ImGui files are being compiled

# Edit the AndroidManifest app_name and activity, strings.xml, and create an extended SDLActivity
# Edit the package names
# Make sure this is on the Java side and not the JNI side

# Need to link ImGui files


# Build and Run commands
./gradlew installDebug

# adb commands:
#  - adb devices, adb logcat