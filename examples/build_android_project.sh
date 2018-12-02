#!/usr/bin/bash

# Learning Resources: 
# https://wiki.libsdl.org/Android
# https://hg.libsdl.org/SDL/file/default/docs/README-android.md

# Java package name: "org.libdearimgui.android"
# cpp switches: -std=c++11 -fexceptions -Wall -Wformat

# TODO list:
# Download Android SDK (26+) and export path so we can use 'tools', 'platform-tools'
# Download Android NDK (r15c+) and export path so we can use 'ndk-build'

SDL2_0_9_Link='https://www.libsdl.org/release/SDL2-2.0.9.tar.gz'

# Example commands for setting up 'Dear ImGui' Android project
wget $SDL2_0_9_Link && tar xzf SDL2-2.0.9.tar.gz -C SDL2
cp -r SDL2/android-project/* ./imgui_example_android
cd imgui_example_android/app/jni
ln -s ../../../SDL2 ./SDL2

# Need to link ImGui files