#ifndef _SOUNDCANVAS_H_
#define _SOUNDCANVAS_H_

#include <memory>

class SoundCanvas {
 public:
  SoundCanvas() { }
  virtual ~SoundCanvas() { }
  
  virtual int play(const std::string & filename) = 0;
  virtual void pause(int streamID) = 0;
  virtual void stop(int streamID) = 0;
  virtual void resume(int streamID) = 0;
  virtual void setVolume(int streamID, float leftVolume, float rightVolume) = 0;

 protected:
  float leftVolume = 1.0f;
  float rightVolume = 1.0f;

 private:
};

class DummySoundCanvas : public SoundCanvas {
 public:
  DummySoundCanvas() { }
  
  int play(const std::string & filename) { return 0; }
  void pause(int streamID) { }
  void stop(int streamID) { }
  void resume(int streamID) { }
  void setVolume(int streamID, float leftVolume, float rightVolume) { }
};

#endif
