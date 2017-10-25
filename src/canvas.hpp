#ifndef CANVAS_H_
#define CANVAS_H_

#include <algorithm>
#include <string>
#include <SDL/SDL.h>

using std::string;

class Canvas {
public:
  Canvas(size_t screenWidth, size_t screenHeight, bool offscreen = false);
  virtual ~Canvas() {};
  void start();
  void makeSnapshot();
  void fillRectangle(const Sint16& x, const Sint16& y, const Uint16& w, const Uint16& h, const Uint8& r, const Uint8& g, const Uint8& b, const Uint8& a);
  void fill(const Uint8& r, const Uint8& g, const Uint8& b, const Uint8& a);
  void update();
  void save(const string& filename);
  SDL_Surface* getSurface() {
    return screen;
  }
  size_t width() {
    return screenWidth;
  }
  size_t height() {
    return screenHeight;
  }
private:
  class SDL_Surface *screen;
  class SDL_Surface *snapshot;

  size_t screenWidth;
  size_t screenHeight;
  bool offscreen;
};

#endif /* CANVAS_H_ */
