#include <jni.h>
#include "SoundCanvas.h"
#include <map>

class AndroidSoundCache {
  public:

  AndroidSoundCache(JNIEnv * _env, jobject _assetManager) {
    _env->GetJavaVM(&javaVM);

    assetManager = _env->NewGlobalRef(_assetManager);

    JNIEnv * env = getJNIEnv();
    soundPoolClass =  (jclass)env->NewGlobalRef(env->FindClass("android/media/SoundPool"));
    soundPoolConstructor = env->GetMethodID(soundPoolClass, "<init>", "(III)V");

    jclass assetManagerClass = env->FindClass("android/content/res/AssetManager");

    managerOpenMethod = env->GetMethodID(assetManagerClass, "openFd", "(Ljava/lang/String;)Landroid/content/res/AssetFileDescriptor;");
    soundLoadMethod = env->GetMethodID(soundPoolClass, "load", "(Landroid/content/res/AssetFileDescriptor;I)I");
    soundPlayMethod = env->GetMethodID(soundPoolClass, "play", "(IFFIIF)I");
    soundPauseMethod = env->GetMethodID(soundPoolClass, "pause", "(I)V");
    soundResumeMethod = env->GetMethodID(soundPoolClass, "resume", "(I)V");
    soundStopMethod = env->GetMethodID(soundPoolClass, "stop", "(I)V");
    soundSetVolumeMethod = env->GetMethodID(soundPoolClass, "setVolume", "(IFF)V");
    soundReleaseMethod = env->GetMethodID(soundPoolClass, "release", "()V");
    env->DeleteLocalRef(assetManagerClass);
    env->ExceptionCheck();
  }
  ~AndroidSoundCache(){

  }

  jobject getAssetManager(){ return assetManager; }



  JNIEnv * getJNIEnv() {
    JNIEnv *Myenv = NULL;
    javaVM->GetEnv((void**)&Myenv, JNI_VERSION_1_6);
    return Myenv;
  }


  jclass soundPoolClass;
  jmethodID soundLoadMethod;
  jmethodID soundPlayMethod;
  jmethodID soundPauseMethod;
  jmethodID soundResumeMethod;
  jmethodID soundStopMethod;
  jmethodID soundSetVolumeMethod;
  jmethodID soundReleaseMethod;
  jmethodID managerOpenMethod;
  jmethodID soundPoolConstructor;
  jobject assetManager;


  private:
  JavaVM * javaVM = 0;
};

class AndroidSoundCanvas : public SoundCanvas {
 public:
  AndroidSoundCanvas(AndroidSoundCache * _cache) {
    cache = _cache;
    JNIEnv * env = cache->getJNIEnv();
    soundPool = env->NewGlobalRef(env->NewObject(cache->soundPoolClass, cache->soundPoolConstructor, 10, 3, 0));
  }

  ~AndroidSoundCanvas() {
    JNIEnv * env = cache->getJNIEnv();
    env->DeleteGlobalRef(cache->soundPoolClass);
  }
  int play(const std::string & filename) override {
    auto it = loadedSounds.find(filename); // look for the filename
    if (it != loadedSounds.end()) {
      int soundID = it->second;
      return playSample(soundID); // sound is already loaded, play it
    } else {
      int soundID = loadSample(filename);
      return playSample(soundID);
    }
  }

  void pause(int streamID) override {
    JNIEnv * env = cache->getJNIEnv();
    env->CallVoidMethod(soundPool, cache->soundPauseMethod, streamID);
  }

  void stop(int streamID) override {
    JNIEnv * env = cache->getJNIEnv();
    env->CallVoidMethod(soundPool, cache->soundStopMethod, streamID);
  }

  void resume(int streamID) override {
    JNIEnv * env = cache->getJNIEnv();
    env->CallVoidMethod(soundPool, cache->soundResumeMethod, streamID);
  }

  void release() {
    JNIEnv * env = cache->getJNIEnv();
    env->CallVoidMethod(soundPool, cache->soundReleaseMethod);
  }

  void setVolume(int streamID, float leftVolume, float rightVolume) override {
    JNIEnv * env = cache->getJNIEnv();
    env->CallVoidMethod(soundPool, cache->soundSetVolumeMethod, streamID, leftVolume * 0.99f, rightVolume * 0.99f);
  }

protected:

  // returns SoundID
  int loadSample(const std::string & filename) {
    JNIEnv * env = cache->getJNIEnv();
    jstring jpath = env->NewStringUTF(filename.c_str());
    jobject file = env->CallObjectMethod(cache->getAssetManager(), cache->managerOpenMethod, jpath);
    int soundID = env->CallIntMethod(soundPool, cache->soundLoadMethod, file, 1);
    loadedSounds[filename] = soundID;
    env->DeleteLocalRef(file);
    env->DeleteLocalRef(jpath);
    return soundID;
  }

  // Returns StreamID
  int playSample(int soundID) {
    JNIEnv * env = cache->getJNIEnv();
    int streamID = env->CallIntMethod(soundPool, cache->soundPlayMethod, soundID, leftVolume, rightVolume, 0, 0, 0.99);
    return streamID;
  }

 private:
  AndroidSoundCache * cache;
  std::map<std::string, int> loadedSounds;
  jobject soundPool;
};
