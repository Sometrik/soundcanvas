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
  virtual void setVolume(int streamID, float leftVolume, float rightVolume);

 protected:
  float leftVolume;
  float rightVolume;

 private:
};

class SoundCanvasFactory {
 public:
  SoundCanvasFactory() { }
  virtual ~SoundCanvasFactory() { }

  virtual std::shared_ptr<SoundCanvas> createCanvas() = 0;
 private:
};
