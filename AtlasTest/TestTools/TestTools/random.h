// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TestTools/random.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2014
 * @brief Very simple random numbers for regression testing.
 *
 * This file provides a few very simple random number generators
 * useful for regression testing.  These are 32-bit LCGs, with constants
 * taken from Numerical Recipes.  These numbers will have poor quality;
 * however, the results should be completely reproducible across platforms.
 * For regression testing, that's often all that's really needed.
 */


#ifndef TESTTOOLS_RANDOM_H
#define TESTTOOLS_RANDOM_H


#include <stdint.h>


namespace Athena_test {


/// Maximum number generated.
static uint32_t rngmax = static_cast<uint32_t> (-1);


/// Generate a random number between 0 and @c rngmax
uint32_t rng_seed (uint32_t& seed)
{
  seed = (1664525*seed + 1013904223);
  return seed;
}


/// Generate a floating-point random number between @c rmin and @c rmax.
float randf_seed (uint32_t& seed, float rmax, float rmin = 0)
{
  return static_cast<float>(rng_seed(seed)) / rngmax * (rmax-rmin) + rmin;
}


/// Generate an integer random number between @c rmin and @c rmax.
int randi_seed (uint32_t& seed, int rmax, int rmin = 0)
{
  return static_cast<int> (randf_seed (seed, rmax, rmin));
}


/// Generator compatible with the STL RandomNumberGenerator.
struct RNG
{
  RNG() : seed(1) {}
  int operator() (int n) const { return randi_seed (seed, n); }
  mutable uint32_t seed;
};


/// Generator compatible with the C++11 STL UniformRandomNumberGenerator.
struct URNG
{
  typedef uint32_t result_type;
  URNG() : seed(1) {}
  static result_type min() { return 0; }
  static result_type max() { return 1000000; }
  result_type operator()() const { return randi_seed (seed, max()); }
  mutable uint32_t seed;
};


uint32_t seed = 1;
uint32_t rng() { return rng_seed(seed); }
int randi (int rmax, int rmin = 0) { return randi_seed (seed, rmax, rmin); }
float randf (float rmax, float rmin = 0) { return randf_seed (seed, rmax, rmin); }



} // namespace Athena_test


#endif // not TESTTOOLS_RANDOM_H
