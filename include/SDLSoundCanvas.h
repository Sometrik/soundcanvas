#include "SoundCanvas.h"
#include <map>

#include <SDL/SDL_mixer.h>

class SDLSoundCanvas : public SoundCanvas {
 public:
  SDLSoundCanvas() { }
  ~SDLSoundCanvas() {
    for (auto & p : loadedSounds) {
      Mix_FreeChunk(p.second);
    }
  }
  int play(const std::string & filename) override {
    auto it = loadedSounds.find(filename); // look for the filename
    if (it != loadedSounds.end()) {
      return playSample(it->second); // sound is already loaded, play it
    } else {
      return playSample(loadSample(filename));
    }
  }

  void pause(int streamID) override {
  }

  void stop(int streamID) override {
  }

  void resume(int streamID) override {
  }

  void release() {
  }

  void setVolume(int streamID, float leftVolume, float rightVolume) override {
  }

protected:
  Mix_Chunk * loadSample(const std::string & filename) {
    std::cerr << "loading " << filename << std::endl;
    checkInit();
    return (loadedSounds[filename] = Mix_LoadWAV(filename.c_str()));
  }

  int playSample(Mix_Chunk * s) {
    checkInit();
    Mix_PlayChannel(-1, s, 0);
    return 0;
  }

  void checkInit() {
    if (!initDone) {
      Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 640);
      initDone = true;
    }
  }

 private:
  std::map<std::string, Mix_Chunk *> loadedSounds;
  bool initDone = false;
};
