// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
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


// Can't use CxxUtils/checker_macros.h here, since that would be circular dependency.
#ifdef ATLAS_GCC_CHECKERS
#define ATLAS_NOT_REENTRANT [[ATLAS::not_reentrant]]
#else
#define ATLAS_NOT_REENTRANT
#endif


#include <stdint.h>
#include <cmath>


namespace Athena_test {


/// Maximum number generated.
static const uint32_t rngmax = static_cast<uint32_t> (-1);


/// Generate a random number between 0 and @c rngmax
inline
uint32_t rng_seed (uint32_t& seed)
{
  seed = (1664525*seed + 1013904223);
  return seed;
}


/// Generate a floating-point random number between @c rmin and @c rmax.
inline
float randf_seed (uint32_t& seed, float rmax, float rmin = 0)
{
  return static_cast<float>(rng_seed(seed)) / static_cast<float>(rngmax) * (rmax-rmin) + rmin;
}


/// Generate an integer random number between @c rmin and @c rmax.
inline
int randi_seed (uint32_t& seed, int rmax, int rmin = 0)
{
  return static_cast<int> (randf_seed (seed, rmax, rmin));
}


/// Generator compatible with the STL RandomNumberGenerator.
struct RNG
{
  RNG() : seed(1) {}
  int operator() (int n) { return randi_seed (seed, n); }
  uint32_t seed;
};


/// Generator compatible with the C++11 STL UniformRandomNumberGenerator.
struct URNG
{
  typedef uint32_t result_type;
  URNG(uint32_t the_seed = 1) : seed(the_seed) {}
  static constexpr result_type min() { return 0; }
  static constexpr result_type max() { return 1000000; }
  result_type operator()() { return randi_seed (seed, max()); }
  uint32_t seed;
};


static uint32_t seed = 1;
inline
uint32_t rng ATLAS_NOT_REENTRANT () { return rng_seed(seed); }
inline
int randi ATLAS_NOT_REENTRANT (int rmax, int rmin = 0) { return randi_seed (seed, rmax, rmin); }
inline
float randf ATLAS_NOT_REENTRANT (float rmax, float rmin = 0) { return randf_seed (seed, rmax, rmin); }


// Distribution objects like in <random>.
// Reimplemented here so that we can be sure of getting reproducible results.

template <class T>
class normal_distribution
{
public:
  normal_distribution (T mu, T s) : m_mu(mu), m_s(s) {}
  T operator() (URNG& rng) const;

private:
  T m_mu;
  T m_s;
};


template <class T>
T normal_distribution<T>::operator() (URNG& rng) const
{
  // Stamdard Box-Muller transform a la Numerical Recipes, except that for simplicity
  // i don't try to remember the second generated number between calls.  I don't
  // really care about performance here.
  T v1, v2, r2;
  do {
    v1 = 2*(static_cast<T>(rng()) / URNG::max())-1;
    v2 = 2*(static_cast<T>(rng()) / URNG::max())-1;
    r2 = v1*v1 + v2*v2;
  } while (r2 >= 1);

  T fac = sqrt(-2*log(r2) / r2);
  return v1*fac*m_s + m_mu;
}


template <class T>
class uniform_real_distribution
{
public:
  uniform_real_distribution (T a, T b) : m_a(a), m_b(b) {}
  T operator() (URNG& rng) const
  {
    return static_cast<T>(rng())/URNG::max()*(m_b-m_a) + m_a;
  }

private:
  T m_a;
  T m_b;
};


} // namespace Athena_test


#endif // not TESTTOOLS_RANDOM_H
