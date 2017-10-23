#include "canvas.hpp"
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include "save_surf.hpp"

Canvas::Canvas(size_t screenWidth, size_t screenHeight, bool offscreen) :
  screen(NULL), screenWidth(screenWidth), screenHeight(screenHeight), offscreen(offscreen) {
  if (screenWidth > 0 && screenHeight > 0) {
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
      printf("Can't init SDL:  %s\n", SDL_GetError());
      exit(1);
    }
    atexit(SDL_Quit);
    if(!offscreen)
      screen = SDL_SetVideoMode(screenWidth, screenHeight, 24, SDL_SWSURFACE);
    else
      screen = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 24, 0, 0, 0, 0);

    if (screen == NULL) {
      printf("Can't set video mode: %s\n", SDL_GetError());
      exit(1);
    }
  }
}

void checkExit() {
  SDL_Event event;
  if(SDL_PollEvent(&event) && event.type == SDL_QUIT) {
    exit(0);
  }
}

void Canvas::fillRectangle(Sint16 x, Sint16 y, Sint16 w, Sint16 h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    checkExit();
    SDL_Rect dst = { x, y , w, h };
    SDL_FillRect(screen, &dst, SDL_MapRGBA(screen->format, r, g, b, a));
}


void Canvas::update() {
  if(!offscreen)
    SDL_Flip(screen);
}

void Canvas::save(const string& filename) {
  png_save_surface(filename.c_str(), screen);
}

