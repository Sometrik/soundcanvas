#ifndef _SOUNDCANVAS_H_
#define _SOUNDCANVAS_H_

#include <memory>

class SoundCanvas {
 public:
  SoundCanvas() { }
  virtual ~SoundCanvas() { }
  
  //returns SoundID
  virtual int loadSound(std::string filename) = 0;
  //Returns StreamID
  virtual int play(int soundID) = 0;
  virtual int play(std::string filename) = 0;
  virtual void pause(int streamID) = 0;
  virtual void stop(int streamID) = 0;
  virtual void resume(int streamID) = 0;
  virtual void setVolume(int streamID, float leftVolume, float rightVolume) = 0;

 protected:
  float leftVolume;
  float rightVolume;

 private:
};

#endif
