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
#include <signal.h>

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

void signal_callback_handler(int signum)
{
  std::cerr << "Caught signal: " << std::to_string(signum) << std::endl;
  CANVAS->save("result/dump.png");
}

void get_pixel(SDL_Surface *surface, const int& x, const int& y, Uint8& r,Uint8& g,Uint8& b,Uint8& a)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	r = p[3];
	g = p[2];
	b = p[1];
	a = p[0];
    } else {
	r = p[0];
	g = p[1];
	b = p[2];
	a = p[3];
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


    Uint8 r, g, b, a;

    for(size_t k = 0; k < w; ++k) {
      for(size_t l = 0; l < h; ++l) {
        get_pixel(CANVAS->getSurface(), x + k, y + l, r ,g, b, a);
        dna[i].r += r;
        dna[i].g += g;
        dna[i].b += b;
      }
    }

    dna[i].r = dna[i].r / (w * h) / 255.0;
    dna[i].g = dna[i].g / (w * h) / 255.0;
    dna[i].b = dna[i].b / (w * h) / 255.0;
    dna[i].a = 0;

    dna[i].r = (dna[i].r + random_real(1.0)) / 2;
    dna[i].g = (dna[i].g + random_real(1.0)) / 2;
    dna[i].b = (dna[i].b + random_real(1.0)) / 2;
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
  mutatedRect = random_int(DNA_TEST.size() - 1);
  double roulette = random_real(2.8);
  double drastic = random_real(2.0);

  // mutate color
  if (roulette < 1) {
    //// completely transparent rects are stupid
    if (DNA_TEST[mutatedRect].a < 0.01  || roulette < 0.25) {
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
    int pointI = random_int(1);
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
    int destination = random_int(DNA_TEST.size() - 1);
    Rectangle r = DNA_TEST[mutatedRect];
    DNA_TEST[mutatedRect] = DNA_TEST[destination];
    DNA_TEST[destination] = r;
    return destination;
  }
  return -1;
}


double diff_pixels(SDL_Surface* testSurf) {
  size_t difference = 0;
  size_t localDiff = 0;
  Uint8 testB;
  Uint8 testG;
  Uint8 testR;
  Uint8 goalB;
  Uint8 goalG;
  Uint8 goalR;
  Uint8* pt = (Uint8*)testSurf->pixels;
  Uint8* pg = (Uint8*)GOAL_SURF->pixels;

  for (size_t i = 0;  i < (HEIGHT * WIDTH); i++) {
    localDiff = 0;
    testR = *pt;
    testG = *(pt + 1);
    testB = *(pt + 2);

    goalR = *pg;
    goalG = *(pg + 1);
    goalB = *(pg + 2);

    localDiff += abs(testR - goalR);
    localDiff += abs(testG - goalG);
    localDiff += abs(testB - goalB);

    difference += localDiff;

    pt += 4;
    pg += 4;
  }

/*  Uint8 a;
  for (size_t y = 0; y < HEIGHT; y++) {
    for (size_t x = 0; x < WIDTH; x++) {
      localDiff = 0;

      get_pixel(testSurf, x, y, testR,testG,testB,a);
      get_pixel(GOAL_SURF, x, y, goalR,goalG,goalB,a);

      localDiff += abs(testR - goalR);
      localDiff += abs(testG - goalG);
      localDiff += abs(testB - goalB);

      difference += localDiff;
    }
  }*/

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
      if(pixDiff < 0.05)
	featDiff = diff_features(CANVAS->getSurface(), GOAL_RGB, FEATURE_MAT);
      else
	featDiff = 1.0;
#else
      featDiff = 1.0;
#endif
//
      if(FEATURE_MAT.cols > 0 && FEATURE_MAT.rows > 0)
	imshow("", FEATURE_MAT);

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
        if(bestStep != 0 && bestStep % 100 == 0) {
          CANVAS->makeSnapshot();
	  init_dna(DNA_BEST, CANVAS->width(), CANVAS->height());
	  std::cerr << "snapshot" << std::endl;
        }
      } else {
        // test sucks, copy best back over test
        DNA_TEST[mutatedRect] = DNA_BEST[mutatedRect];

        if (otherMutated >= 0)
          DNA_TEST[otherMutated] = DNA_BEST[otherMutated];
      }

      testStep++;

      int invisible = 0;
      for(size_t i = 0; i < DNA_BEST.size(); ++i) {
        if(DNA_BEST[i].a == 0.0)
          ++invisible;
      }
      if (testStep != 0 && testStep % 1000 == 0) {
        std::cout << fitness << '\t' << 1000000.0/(microseconds/1000) << '\t' << invisible << '\t' << testStep << '\t' << boost::filesystem::path(filename).stem().string() << std::endl;
        std::cout.flush();
        microseconds = 0;
      }
    };
    microseconds += tt.measure(f);
   }

  string prefix = boost::filesystem::path(filename).stem().string();
  std::stringstream ssname;
  ssname << "result/" << prefix << "_result.png";
  draw_dna(DNA_BEST, CANVAS);
  CANVAS->save(ssname.str());

  /*
  ssname.str("");
  ssname << "result/" << filename << "_" << std::setfill('0') << std::setw(10) << "_feature.png";
  if(FEATURE_MAT.rows > 0)
    imwrite(ssname.str(), FEATURE_MAT);*/

  /*ssname.str("");
  ssname << "result/" << prefix << ".dna";
  std::ofstream of_dna(ssname.str());
  boost::archive::binary_oarchive oa(of_dna);
  oa << DNA_BEST;*/
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
        int thispixel = y * WIDTH * 4 + x * 4;
        goal_b = GOAL_DATA[thispixel];
        GOAL_DATA[thispixel] = GOAL_DATA[thispixel + 2];
        GOAL_DATA[thispixel + 2] = goal_b;
      }
    }

    GOAL_RGB = Mat(WIDTH,HEIGHT,CV_8UC4,GOAL_DATA,GOAL_SURF->pitch);
    CANVAS = new Canvas(WIDTH, HEIGHT, true);
    CANVAS->fill(0,0,0,255);
    CANVAS->makeSnapshot();
    if (argc == 5) {
      std::cerr << argv[4] << std::endl;
      std::ifstream if_dna(argv[4]);
      boost::archive::binary_iarchive ia(if_dna);
      ia >> DNA_BEST;
    } else {
      init_dna(DNA_BEST, WIDTH, HEIGHT);
    }
    signal(SIGUSR1, signal_callback_handler);
    loop(numIterations, filename);
  }
}
