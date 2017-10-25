#include "draw.hpp"
#include "dna.hpp"
#include "canvas.hpp"
#include <cmath>

void draw_rect(const Rectangle& rect, Canvas* canvas) {
  if(rect.a < 0.01)
    return;
  int x = rect.points[0].x;
  int y = rect.points[0].y;
  int w = abs(rect.points[1].x - x);
  int h = abs(rect.points[1].y - y);
  canvas->fillRectangle(x,y, w,h, round(rect.r * 255), round(rect.g * 255), round(rect.b * 255), round(rect.a * 255));
}

void draw_dna(Dna& dna, Canvas* canvas) {
  canvas->start();

  for (size_t i = 0; i < dna.size(); i++) {
    draw_rect(dna[i], canvas);
  }
  canvas->update();
}
