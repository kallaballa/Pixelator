/*
 * helpers.hpp
 *
 *  Created on: Jun 2, 2017
 *      Author: elchaschab
 */

#ifndef SRC_HELPERS_HPP_
#define SRC_HELPERS_HPP_

std::random_device rd;     // only used once to initialise (seed) engine
std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)

template <typename T> T clamp(T val, T min, T max) {
  return ((val) < (min) ? (min) : (val) > (max) ? (max) : (val));
}

template <typename T> T random_int(T max) {
  typename std::uniform_int_distribution<T> uni(0,max); // guaranteed unbiased

  return uni(rng);
}

template <typename T> T random_real(T max) {
  typename std::uniform_real_distribution<T> uni(0,max); // guaranteed unbiased

  return uni(rng);
}

#endif /* SRC_HELPERS_HPP_ */
