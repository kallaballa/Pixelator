#include <cmath>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <random>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>

#include "canvas.hpp"
#include "features.hpp"
#include "helpers.hpp"
#include "dna.hpp"
#include "draw.hpp"
#include "time_tracker.hpp"

using std::vector;

size_t WIDTH;
size_t HEIGHT;
unsigned char * GOAL_DATA = NULL;
SDL_Surface * GOAL_SURF;
Mat FEATURE_MAT;
Canvas* CANVAS;
Mat GOAL_RGB;
Dna DNA_BEST(1);
Dna DNA_TEST(1);

int mutatedRect;

Uint32 get_pixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void init_dna(Dna& dna, size_t WIDTH, size_t HEIGHT) {
  for (size_t i = 0; i < dna.size(); i++) {
    for (int j = 0; j < 2; j++) {
      dna[i].points[j].x = random_real((double)WIDTH);
      dna[i].points[j].y = random_real((double)HEIGHT);
    }
/*
    size_t x = min(dna[i].points[0].x,dna[i].points[1].x);
    size_t y = min(dna[i].points[0].y,dna[i].points[1].y);

    size_t w = round(fabs(dna[i].points[0].x - dna[i].points[1].x));
    size_t h = round(fabs(dna[i].points[0].y - dna[i].points[1].y));


    Uint8 r, g, b;

    for(size_t k = 0; k < w; ++k) {
      for(size_t l = 0; l < h; ++l) {
        Uint32 p = get_pixel(CANVAS->getSurface(), x + k, y + l);
        SDL_GetRGB(p, CANVAS->getSurface()->format, &r, &g, &b);
        dna[i].r += r;
        dna[i].g += g;
        dna[i].b += b;
      }
    }

    dna[i].r = dna[i].r / (w * h) / 255.0;
    dna[i].g = dna[i].g / (w * h) / 255.0;
    dna[i].b = dna[i].b / (w * h) / 255.0;
    dna[i].a = 0;
*/
    dna[i].r = random_real(1.0);
    dna[i].g = random_real(1.0);
    dna[i].b = random_real(1.0);
    dna[i].a = 0;

/*  if(random_real(1.0) > 0.5) {
      dna[i].r = 0;
      dna[i].g = 0;
      dna[i].b = 1.0;
    } else {
      dna[i].r = 1.0;
      dna[i].g = 1.0;
      dna[i].b = 1.0;
    }
    dna[i].a = 0;*/
  }
}
int mutate(void) {
  mutatedRect = random_int(DNA_TEST.size());
  double roulette = random_real(2.8);
  double drastic = random_real(2.0);

  // mutate color
  if (roulette < 1) {
    //DNA_TEST[mutatedRect].a < 0.01 // completely transparent rects are stupid
    if (roulette < 0.25) {
      if (drastic < 1) {
        DNA_TEST[mutatedRect].a += random_real(0.1);
        DNA_TEST[mutatedRect].a = clamp(DNA_TEST[mutatedRect].a, 0.0, 1.0);
      } else
        DNA_TEST[mutatedRect].a = random_real(1.0);
    } else if (roulette < 0.50) {
      if (drastic < 1) {
        DNA_TEST[mutatedRect].r += random_real(0.1);
        DNA_TEST[mutatedRect].r = clamp(DNA_TEST[mutatedRect].r, 0.0, 1.0);
      } else
        DNA_TEST[mutatedRect].r = random_real(1.0);
    } else if (roulette < 0.75) {
      if (drastic < 1) {
        DNA_TEST[mutatedRect].g += random_real(0.1);
        DNA_TEST[mutatedRect].g = clamp(DNA_TEST[mutatedRect].g, 0.0, 1.0);
      } else
        DNA_TEST[mutatedRect].g = random_real(1.0);
    } else {
      if (drastic < 1) {
        DNA_TEST[mutatedRect].b += random_real(0.1);
        DNA_TEST[mutatedRect].b = clamp(DNA_TEST[mutatedRect].b, 0.0, 1.0);
      } else
        DNA_TEST[mutatedRect].b = random_real(1.0);
    }
  }

  // mutate rect
  else if (roulette < 2.0) {
    int pointI = random_int(2);
    if (roulette < 1.5) {
      if (drastic < 1) {
        DNA_TEST[mutatedRect].points[pointI].x += random_real(WIDTH/10.0);
        DNA_TEST[mutatedRect].points[pointI].x = clamp(DNA_TEST[mutatedRect].points[pointI].x, 0.0, (double)WIDTH - 1);
      } else
        DNA_TEST[mutatedRect].points[pointI].x = random_real((double)WIDTH);
    } else {
      if (drastic < 1) {
        DNA_TEST[mutatedRect].points[pointI].y += random_real(HEIGHT/10.0);
        DNA_TEST[mutatedRect].points[pointI].y = clamp(DNA_TEST[mutatedRect].points[pointI].y, 0.0, (double)HEIGHT - 1);
      } else
        DNA_TEST[mutatedRect].points[pointI].y = random_real((double)HEIGHT);
    }
  }
 // mutate stacking
  else {
    int destination = random_int(DNA_TEST.size());
    Rectangle r = DNA_TEST[mutatedRect];
    DNA_TEST[mutatedRect] = DNA_TEST[destination];
    DNA_TEST[destination] = r;
    return destination;
  }
  return -1;
}

int MAX_FITNESS = -1;

double diff_pixels(SDL_Surface* testSurf) {
  unsigned char * testData = (unsigned char *)testSurf->pixels;

  int difference = 0;

  int maxFitness = 0;
  int maxDiff = 0;
  for (size_t y = 0; y < HEIGHT; y++) {
    for (size_t x = 0; x < WIDTH; x++) {
      int thispixel = y * WIDTH * 3 + x * 3;

      unsigned char testB = testData[thispixel];
      unsigned char testG = testData[thispixel + 1];
      unsigned char testR = testData[thispixel + 2];

      unsigned char goalB = GOAL_DATA[thispixel];
      unsigned char goalG = GOAL_DATA[thispixel + 1];
      unsigned char goalR = GOAL_DATA[thispixel + 2];

      if (MAX_FITNESS == -1)
        maxFitness += goalR + goalG + goalB;

      int localDiff = 0;
      localDiff += abs(testR - goalR);
      localDiff += abs(testG - goalG);
      localDiff += abs(testB - goalB);

      if (localDiff > maxDiff) {
        maxDiff = localDiff;
      }

      difference += localDiff;
    }
  }

  if (MAX_FITNESS == -1)
    MAX_FITNESS = maxFitness;
  return ((double)difference / (double)(HEIGHT * WIDTH * 4.0)) / (double)255.0;
}

static void loop(size_t numIterations, string filename) {
  DNA_TEST = DNA_BEST;
  TimeTracker& tt = *TimeTracker::getInstance();
  size_t microseconds = 0;
  double lowestFeat = std::numeric_limits<double>().max();
  double lowestPix = std::numeric_limits<double>().max();
  double lowestDiff = std::numeric_limits<double>().max();
  double fitness;
  int testStep = 0;
  int bestStep = 0;

  for (size_t i = 0; i < numIterations; ++i) {
    auto f = [&]() {
      off_t otherMutated = mutate();
      while(DNA_TEST[mutatedRect].a == 0 && !(otherMutated > 0 && DNA_TEST[otherMutated].a != 0) )
        otherMutated = mutate();

      draw_dna(DNA_TEST, CANVAS);

      fitness = (1.0 - lowestDiff) * 100;

      double pixDiff = diff_pixels(CANVAS->getSurface());
      assert(pixDiff <= 1.0 && pixDiff >= 0.0);

      double featDiff;
#ifndef _NO_OPENCV
      if(pixDiff < 0.00)
        featDiff = diff_features(CANVAS->getSurface(), GOAL_RGB, FEATURE_MAT);
      else
#endif
	featDiff = 1.0;

      assert(featDiff <= 1.0 && featDiff >= 0.0);

      double diff = (pixDiff + featDiff) / 2;

      if (diff <= lowestDiff) {
        lowestFeat = featDiff;
        lowestPix = pixDiff;
        ++bestStep;
        // test is good, copy to best
        DNA_BEST[mutatedRect] = DNA_TEST[mutatedRect];
        if (otherMutated >= 0)
          DNA_BEST[otherMutated] = DNA_TEST[otherMutated];

        lowestDiff = diff;

      } else {
        // test sucks, copy best back over test
        DNA_TEST[mutatedRect] = DNA_BEST[mutatedRect];

        if (otherMutated >= 0)
          DNA_TEST[otherMutated] = DNA_BEST[otherMutated];
      }

      testStep++;

      int invisible = 0;
      for(size_t i = 0; i < DNA_BEST.size(); ++i) {
        if(DNA_BEST[i].a == 0)
          ++invisible;
      }
      if (testStep != 0 && testStep % 100 == 0) {
        std::cout << fitness << '\t' << 1000000.0/(microseconds/100) << '\t' << invisible << '\t' << testStep << std::endl;
        std::cout.flush();
        microseconds = 0;
      }
    };
    microseconds += tt.measure(f);
   }

  string prefix = boost::filesystem::path(filename).stem().string();
  std::stringstream ssname;
  ssname << "result/" << prefix << "_result.png";
  CANVAS->save(ssname.str());

  /*
  ssname.str("");
  ssname << "result/" << filename << "_" << std::setfill('0') << std::setw(10) << "_feature.png";
  if(FEATURE_MAT.rows > 0)
    imwrite(ssname.str(), FEATURE_MAT);*/

  ssname.str("");
  ssname << "result/" << prefix << ".dna";
  std::ofstream of_dna(ssname.str());
  boost::archive::binary_oarchive oa(of_dna);
  oa << DNA_BEST;
}

int main(int argc, char ** argv) {
  if (argc < 4 || argc > 5)
    std::cerr << "Usage: pixelator <number-of-rectangles> <number-of-iterations> <png-file> [<dna-file>]" << std::endl;
  else {
    size_t numShapes = std::stoi(string(argv[1]));
    size_t numIterations = std::stoi(string(argv[2]));
    string filename = string(argv[3]);
    DNA_BEST.resize(numShapes);
    DNA_TEST.resize(numShapes);
    GOAL_SURF = IMG_Load(argv[3]);
    GOAL_DATA = (unsigned char *)GOAL_SURF->pixels;
    WIDTH = GOAL_SURF->w;
    HEIGHT = GOAL_SURF->h;

    // fix pixel format
    unsigned char goal_b;
    for (size_t y = 0; y < HEIGHT; y++) {
      for (size_t x = 0; x < WIDTH; x++) {
        int thispixel = y * WIDTH * 3 + x * 3;
        goal_b = GOAL_DATA[thispixel];
        GOAL_DATA[thispixel] = GOAL_DATA[thispixel + 2];
        GOAL_DATA[thispixel + 2] = goal_b;
      }
    }

    GOAL_RGB = Mat(WIDTH,HEIGHT,CV_8UC3,GOAL_DATA,GOAL_SURF->pitch);
    CANVAS = new Canvas(WIDTH, HEIGHT, true);
    if (argc == 5) {
      std::cerr << argv[4] << std::endl;
      std::ifstream if_dna(argv[4]);
      boost::archive::binary_iarchive ia(if_dna);
      ia >> DNA_BEST;
    } else {
      init_dna(DNA_BEST, WIDTH, HEIGHT);
    }

    loop(numIterations, filename);
  }
}
