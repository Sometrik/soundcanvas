#include <jni.h>
#include "SoundCanvas.h"
#include <map>


class AndroidSoundCanvas : public SoundCanvas {
 public:
  AndroidSoundCanvas(JNIEnv * _env, jobject _assetManager) : SoundCanvas(){
    _env->GetJavaVM(&javaVM);
    assetManager = _assetManager;
    _env->NewGlobalRef(assetManager);
  }

  ~AndroidSoundCanvas(){
    JNIEnv * env = getJNIEnv();
    if (initDone) {
      env->DeleteGlobalRef(soundPoolClass);
      env->DeleteGlobalRef(assetManager);
    }
  }
  int play(const std::string & filename) override {
    checkInit();
    auto it = loadedSounds.find(filename); // look for the filename
    if (it != loadedSounds.end()) {
      int soundID = it->second;
      return playSample(soundID); // sound is already loaded, play it
    } else {
      int soundID = loadSound(filename);
      return playSample(soundID);
    }
  }

  void pause(int streamID) {
    checkInit();
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPool, soundPauseMethod, streamID);
  }

  void stop(int streamID) {
    checkInit();
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPool, soundStopMethod, streamID);
  }

  void resume(int streamID) {
    checkInit();
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPool, soundResumeMethod, streamID);
  }

  void release() {
    checkInit();
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPool, soundReleaseMethod);
  }

  void setVolume(int streamID, float leftVolume, float rightVolume) {
    checkInit();
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPool, soundSetVolumeMethod, streamID, leftVolume * 0.99f, rightVolume * 0.99f);
  }

protected:

  // returns SoundID
  int loadSound(const std::string & filename) {
    checkInit();
    JNIEnv * env = getJNIEnv();
    jstring jpath = env->NewStringUTF(filename.c_str());
    jobject file = env->CallObjectMethod(assetManager, managerOpenMethod, jpath);
    int soundID = env->CallIntMethod(soundPool, soundLoadMethod, file, 1);
    loadedSounds[filename] = soundID;
    return soundID;
  }

  //Returns StreamID
  int playSample(int soundID) {
    checkInit();
    JNIEnv * env = getJNIEnv();
    int streamID = env->CallIntMethod(soundPool, soundPlayMethod, soundID, leftVolume, rightVolume, 0, 0, 0.99);
    return streamID;
  }

  void checkInit() {
    if (!initDone) {
      androidInit();
    }
  }

  void androidInit() {
    JNIEnv * env = getJNIEnv();
    soundPoolClass =  (jclass)env->NewGlobalRef(env->FindClass("android/media/SoundPool"));
     jmethodID soundPoolConstructor = env->GetMethodID(soundPoolClass, "<init>", "(III)V");
     soundPool = env->NewGlobalRef(env->NewObject(soundPoolClass, soundPoolConstructor, 10, 3, 0));

     jclass assetManagerClass = env->FindClass("android/content/res/AssetManager");

     managerOpenMethod = env->GetMethodID(assetManagerClass, "openFd", "(Ljava/lang/String;)Landroid/content/res/AssetFileDescriptor;");
     soundLoadMethod = env->GetMethodID(soundPoolClass, "load", "(Landroid/content/res/AssetFileDescriptor;I)I");
     soundPlayMethod = env->GetMethodID(soundPoolClass, "play", "(IFFIIF)I");
     soundPauseMethod = env->GetMethodID(soundPoolClass, "pause", "(I)V");
     soundResumeMethod = env->GetMethodID(soundPoolClass, "resume", "(I)V");
     soundStopMethod = env->GetMethodID(soundPoolClass, "stop", "(I)V");
     soundSetVolumeMethod = env->GetMethodID(soundPoolClass, "setVolume", "(IFF)V");
     soundReleaseMethod = env->GetMethodID(soundPoolClass, "release", "()V");
     env->ExceptionCheck();

     initDone = true;
   }

  JNIEnv * getJNIEnv() {
    JNIEnv *Myenv = NULL;
    javaVM->GetEnv((void**)&Myenv, JNI_VERSION_1_6);
    return Myenv;
  }

  

 private:
  jclass soundPoolClass;
  jmethodID soundLoadMethod;
  jmethodID soundPlayMethod;
  jmethodID soundPauseMethod;
  jmethodID soundResumeMethod;
  jmethodID soundStopMethod;
  jmethodID soundSetVolumeMethod;
  jmethodID soundReleaseMethod;
  jmethodID managerOpenMethod;
  jobject soundPool;
  jobject assetManager;
  bool initDone = false;
  int priority = 1;
  JavaVM * javaVM;
  std::map<std::string, int> loadedSounds;
};
