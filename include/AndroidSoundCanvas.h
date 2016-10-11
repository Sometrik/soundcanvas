#include <jni.h>
#include "SoundCanvas.h"

class AndroidSoundCanvas : public SoundCanvas {
 public:
  AndroidSoundCanvas(JNIEnv * _env){
    _env->GetJavaVM(&javaVM);
  }

  ~AndroidSoundCanvas(){
    JNIEnv * env = getJNIEnv();
    if (initDone){
      env->DeleteGlobalRef(soundPoolClass);
    }
  }
  
  void androidInit() {
    JNIEnv * env = getJNIEnv();
     soundPoolClass =  (jclass)env->NewGlobalRef(env->FindClass("android/media/SoundPool"));

     soundLoadMethod = env->GetMethodID(soundPoolClass, "load", "(Ljava/lang/String;I)I");
     soundPlayMethod = env->GetMethodID(soundPoolClass, "play", "(IFFIIF)I");
     soundPauseMethod = env->GetMethodID(soundPoolClass, "pause", "(I)V");
     soundResumeMethod = env->GetMethodID(soundPoolClass, "resume", "(I)V");
     soundStopMethod = env->GetMethodID(soundPoolClass, "stop", "(I)V");
     soundSetVolumeMethod = env->GetMethodID(soundPoolClass, "setVolume", "(IFF)V");
     soundReleaseMethod = env->GetMethodID(soundPoolClass, "release", "()V");

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
    int soundID = env->CallIntMethod(soundPoolClass, soundLoadMethod, filePath, 1);
    return soundID;
  }

  //Returns StreamID
  int play(int soundID){
    JNIEnv * env = getJNIEnv();
    int streamID = env->CallIntMethod(soundPoolClass, soundPlayMethod, leftVolume, rightVolume, 1, 0, 1);
    return streamID;
  }

  void pause(int streamID){
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPoolClass, soundPauseMethod, streamID);
  }

  void stop(int streamID){
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPoolClass, soundStopMethod, streamID);
  }

  void resume(int streamID){
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPoolClass, soundResumeMethod, streamID);
  }

  void release(){
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPoolClass, soundReleaseMethod);
  }

  void setVolume(int streamID, float leftVolume, float rightVolume){
    JNIEnv * env = getJNIEnv();
    env->CallVoidMethod(soundPoolClass, soundSetVolumeMethod, streamID, leftVolume, rightVolume);
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
  bool initDone = false;
  int priority = 1;
  JavaVM * javaVM;
};

class AndroidSoundCanvasFactory : public SoundCanvasFactory  {
 public:
  AndroidSoundCanvasFactory(JNIEnv * _env) : env(_env) { }
  ~AndroidSoundCanvasFactory() { }

  std::shared_ptr<SoundCanvas> createCanvas() {
    return std::make_shared < AndroidSoundCanvas > (env);
  }
 private:
  JNIEnv * env;
};
