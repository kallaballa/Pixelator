/*
 * dna.hpp
 *
 *  Created on: Jun 2, 2017
 *      Author: elchaschab
 */

#ifndef SRC_DNA_HPP_
#define SRC_DNA_HPP_

#include <boost/serialization/vector.hpp>

typedef struct {
  friend class boost::serialization::access;

  double x, y;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & x;
    ar & y;
  }
} point_t;

class Rectangle {
  friend class boost::serialization::access;
public:
  double r, g, b, a;
  point_t points[2];
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & r;
    ar & g;
    ar & b;
    ar & a;
    ar & points;
  }
};

struct Dna: public std::vector<Rectangle> {
  friend class boost::serialization::access;

  Dna(size_t size) :
      std::vector<Rectangle>(size) {
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & *((vector<Rectangle>*) this);
  }
};

#endif /* SRC_DNA_HPP_ */
