/*
 * draw.hpp
 *
 *  Created on: Jun 2, 2017
 *      Author: elchaschab
 */

#ifndef SRC_DRAW_HPP_
#define SRC_DRAW_HPP

class Rectangle;
class Dna;
class Canvas;

void draw_rect(const Rectangle& rect, Canvas* canvas);
void draw_dna(Dna& dna, Canvas* canvas);

#endif /* SRC_DRAW_HPP_ */
