# Android + ImGui + SDL2 + OpenGL ES 2 example
This project shows the ImGui test window example using Android.
OpenGL ES 2 is required. OpenGL ES 3 features like Vertex Array Objects could be used as well
for devices that support OpenGL ES 3. Note that you need to change the version requirements in the Gradle builds files 
and the AndroidManifest.

[Example Screenshot](http://i.imgur.com/t6nTbyX.jpg)

## Instructions
1. Download [Android Studio and Android SDK tools](https://developer.android.com/sdk/index.html), 
and the [Android NDK](https://developer.android.com/ndk/downloads/index.html) (tested with NDK-r10e).
2. Export the Android SDK and NDK PATH ```c export PATH=$PATH:/home/<usr>/Android/android-ndk-r10e```
3. Open up Android Studio terminal and change directories to JNI ```c cd android_example/app/src/main/jni```
4. Now create a symbolic link to the SDL2 libraries on your system. For instance, on Linux, ```c ln -s <path-to-SDL2-root> SDL2```
Additionally you can just copy and paste the SDL2 directory into the jni directory as well. It is important that
the SDL2 directory or link is specifically called "SDL2".
4. Run ```c ndk-build``` from the Android Studio terminal.
5. Connect your Android device (emulator most likely will not work!), run the app, and select your device.

## Additional Information and Resources
 - [SDL Android Readme](https://hg.libsdl.org/SDL/file/default/docs/README-android.md)