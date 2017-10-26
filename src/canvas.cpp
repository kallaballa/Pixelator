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

void Canvas::blitRect(uint x, uint y, uint w, uint h, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  SDL_Surface *sfc = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, SDL_SWSURFACE);
  SDL_FillRect(sfc, NULL, SDL_MapRGBA(screen->format, r, g ,b, a));
  SDL_SetAlpha(sfc, SDL_SRCALPHA, a);

  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;
  SDL_BlitSurface(sfc, NULL, screen, &rect);

  SDL_FreeSurface(sfc);
}

void checkExit() {
  SDL_Event event;
  if(SDL_PollEvent(&event) && event.type == SDL_QUIT) {
    exit(0);
  }
}

void Canvas::start() {
  memcpy(screen->pixels, snapshot->pixels, width() * height() * screen->format->BytesPerPixel);
}

void Canvas::makeSnapshot() {
  memcpy(snapshot->pixels, screen->pixels, width() * height() * screen->format->BytesPerPixel);
}

void Canvas::fillRectangle(const Sint16& x, const Sint16& y, const Uint16& w, const Uint16& h, const Uint8& r, const Uint8& g, const Uint8& b, const Uint8& a) {
    checkExit();
    /*Sint16 xv[4] = { x, x + w, x + w, x };
    Sint16 yv[4] = { y, y, y + h,  y + h};
    filledPolygonRGBA(screen,xv,yv,4,r,g,b,a);*/
    blitRect(x,y,w,h,r,g,b,a);
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

