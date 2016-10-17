#include <jni.h>
#include "SoundCanvas.h"
#include <AndroidLogger.h>


class AndroidSoundCanvas : public SoundCanvas {
 public:
  AndroidSoundCanvas(JNIEnv * _env, jobject _assetManager) : SoundCanvas(){
    _env->GetJavaVM(&javaVM);
    assetManager = _assetManager;
    _env->NewGlobalRef(assetManager);
    leftVolume = 1.0f;
    rightVolume = 1.0f;
    androidInit();
  }

  ~AndroidSoundCanvas(){
    JNIEnv * env = getJNIEnv();
    if (initDone){
      env->DeleteGlobalRef(soundPoolClass);
      env->DeleteGlobalRef(assetManager);
    }
  }
  
  void androidInit() {
    JNIEnv * env = getJNIEnv();
    auto logger = AndroidLogger();
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

  //returns SoundID
  int loadSound(const char * filePath) {
    JNIEnv * env = getJNIEnv();
    jstring jpath = env->NewStringUTF(filePath);
    jobject file = env->CallObjectMethod(assetManager, managerOpenMethod, jpath);
    int soundID = env->CallIntMethod(soundPool, soundLoadMethod, file, 1);
    return soundID;
  }

  //Returns StreamID
  int play(int soundID){
    JNIEnv * env = getJNIEnv();
    int streamID = env->CallIntMethod(soundPool, soundPlayMethod, soundID, leftVolume, rightVolume, 0, 0, 0.99);
    return streamID;
  }

  void pause(int streamID){
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPool, soundPauseMethod, streamID);
  }

  void stop(int streamID){
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPool, soundStopMethod, streamID);
  }

  void resume(int streamID){
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPool, soundResumeMethod, streamID);
  }

  void release(){
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPool, soundReleaseMethod);
  }

  void setVolume(int streamID, float leftVolume, float rightVolume){
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPool, soundSetVolumeMethod, streamID, leftVolume * 0.99f, rightVolume * 0.99f);
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
};
