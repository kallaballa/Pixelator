#include "canvas.hpp"
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include "save_surf.hpp"

Canvas::Canvas(size_t screenWidth, size_t screenHeight, bool offscreen) :
  screen(NULL), snapshot(NULL), screenWidth(screenWidth), screenHeight(screenHeight), offscreen(offscreen) {
  if (screenWidth > 0 && screenHeight > 0) {

    atexit(SDL_Quit);
    if(!offscreen) {
      if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        printf("Can't init SDL:  %s\n", SDL_GetError());
        exit(1);
      }
      screen = SDL_SetVideoMode(screenWidth, screenHeight, 32, SDL_SWSURFACE);
    }
    else
      screen = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32, 0, 0, 0, SDL_SWSURFACE);

    snapshot = SDL_CreateRGBSurface(0, screenWidth, screenHeight, 32, 0, 0, 0, SDL_SWSURFACE);
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

void Canvas::start() {
  SDL_UpperBlit(snapshot, NULL, screen, NULL);
}

void Canvas::makeSnapshot() {
  SDL_UpperBlit(screen, NULL, snapshot, NULL);
}

void Canvas::fillRectangle(const Sint16& x, const Sint16& y, const Uint16& w, const Uint16& h, const Uint8& r, const Uint8& g, const Uint8& b, const Uint8& a) {
    checkExit();
    Sint16 xv[4] = { x, x + w, x + w, x };
    Sint16 yv[4] = { y, y, y + h,  y + h};
    filledPolygonRGBA(screen,xv,yv,4,r,g,b,a);
}


void Canvas::fill(const Uint8& r, const Uint8& g, const Uint8& b, const Uint8& a) {
    checkExit();
    SDL_FillRect(screen, NULL, SDL_MapRGBA(screen->format,r,g,b,a));
}

void Canvas::update() {
  if(!offscreen)
    SDL_Flip(screen);
}

void Canvas::save(const string& filename) {
  png_save_surface(filename.c_str(), screen);
}

