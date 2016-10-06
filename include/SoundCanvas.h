
class SoundCanvas {
 public:
  SoundCanvas();
  virtual ~SoundCanvas() { }
  
  //returns SoundID
  virtual int loadSound(const char * filePath);
  //Returns StreamID
  virtual int play(int soundID);
  virtual void pause(int streamID);
  virtual void stop(int streamID);
  virtual void resume(int streamID);
  virtual void setPriority(int streamID, int priority);
  virtual void setVolume(int streamID, float leftVolume, float rightVolume);

 protected:
  float leftVolume;
  float rightVolume;

 private:
};
