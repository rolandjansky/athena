/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/PackedContainer_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2014
 * @brief Regression tests for PackedContainer
 */

#undef NDEBUG
#include "AthContainers/PackedConverter.h"
#include "TestTools/random.h"
#include "TestTools/FLOATassert.h"
#include "CxxUtils/ones.h"
#include <typeinfo>
#include <vector>
#include <iostream>
#include <cassert>


struct OutputArgs
{
  OutputArgs() : ipos(0) {}
  std::vector<uint32_t> vec;
  size_t ipos;
  OutputArgs& operator<< (uint32_t x) { vec.push_back(x); return *this; }
  OutputArgs& operator>> (uint32_t& x) { x = vec[ipos++]; return *this; }
};


template <class T>
void fill_cont (uint32_t& seed,
                size_t nbits, std::vector<T>& cont, size_t n, bool is_signed)
{
  for (size_t i = 0; i < n; i++) {
    if (is_signed) {
      int max = CxxUtils::ones<uint32_t> (nbits-1);
      if (nbits < 30) max *= 2;
      if (nbits < 29) max *= 2;
      cont.push_back (static_cast<T> (Athena_test::randi_seed (seed, max, -max-1)));
    }
    else
      cont.push_back (static_cast<T> (Athena_test::rng_seed (seed)));
  }
}


template <class T>
void fill_float_cont (uint32_t& seed,
                      size_t /*nbits*/, std::vector<T>& cont, size_t n, bool is_signed)
{
  for (size_t i = 0; i < n; i++) {
    if (is_signed)
      cont.push_back (Athena_test::randf_seed (seed, 200) - 100);
    else
      cont.push_back (Athena_test::randf_seed (seed, 100));
  }
}


void fill_cont (uint32_t& seed,
                size_t nbits, std::vector<float>& cont, size_t n, bool is_signed)
{
  fill_float_cont (seed, nbits, cont, n, is_signed);
}


void fill_cont (uint32_t& seed,
                size_t nbits, std::vector<double>& cont, size_t n, bool is_signed)
                
{
  fill_float_cont (seed, nbits, cont, n, is_signed);
}


template <class T>
void fill_cont (uint32_t& seed,
                size_t nbits, std::vector<std::vector<T> >& cont, size_t n, bool is_signed)
{
  cont.resize (n);
  for (size_t i = 0; i < n; i++) {
    size_t nelt = Athena_test::randi_seed (seed, 30);
    fill_cont (seed, nbits, cont[i], nelt, is_signed);
    assert (cont[i].size() == nelt);
  }
}


void compare_signed_int (size_t nbits, int x1, int x2)
{
  int max = CxxUtils::ones<uint32_t> (nbits-1);
  int min = - max-1;

  if (x1 > max)
    assert (x2 == max);
  else if (x1 < min)
    assert (x2 == min);
  else
    assert (x1 == x2);
}


template <class T1, class T2>
void compare_int_vec (size_t nbits,
                      const std::vector<T1>& c1,
                      const std::vector<T2>& c2)
{
  uint32_t mask = CxxUtils::ones<uint32_t> (nbits);
  assert (c1.size() == c2.size());
  for (size_t i = 0; i < c1.size(); i++) {
    //std::cout << c1[i] << " " << c2[i] << "\n";
    if (std::numeric_limits<T2>::is_signed)
      compare_signed_int (nbits, c1[i], c2[i]);
    else {
      uint32_t val1 = static_cast<uint32_t> (c1[i]);
      assert (static_cast<T2> (mask & val1) == c2[i]);
    }
  }
}


template <class T>
void compare_float_vec (size_t nmantissa,
                        const std::vector<T>& c1,
                        const std::vector<T>& c2)
{
  float thresh = 1;
  for (size_t i = 0; i < nmantissa; i++)
    thresh /= 2;
  assert (c1.size() == c2.size());
  for (size_t i = 0; i < c1.size(); i++) {
    assert (Athena_test::isEqual (c1[i], c2[i], thresh));
  }
}


template <class T1, class T2>
void compare_cont (const SG::PackedParameters& parms,
                   const std::vector<T1>& c1,
                   const std::vector<T2>& c2)
{
  compare_int_vec (parms.nbits(), c1, c2);
}


void compare_cont (const SG::PackedParameters& parms,
                   const std::vector<float>& c1,
                   const std::vector<float>& c2)
{
  compare_float_vec (parms.nmantissa(), c1, c2);
}


void compare_cont (const SG::PackedParameters& parms,
                   const std::vector<double>& c1,
                   const std::vector<double>& c2)
{
  compare_float_vec (parms.nmantissa(), c1, c2);
}


template <class T1, class T2>
void compare_cont (const SG::PackedParameters& parms,
                   const std::vector<std::vector<T1> >& c1,
                   const std::vector<std::vector<T2> >& c2)
{
  assert (c1.size() == c2.size());
  for (size_t i = 0; i < c1.size(); i++) {
    compare_int_vec (parms.nbits(), c1[i], c2[i]);
  }
}


void compare_cont (const SG::PackedParameters& parms,
                   const std::vector<std::vector<float> >& c1,
                   const std::vector<std::vector<float> >& c2)
{
  assert (c1.size() == c2.size());
  for (size_t i = 0; i < c1.size(); i++) {
    compare_float_vec (parms.nmantissa(), c1[i], c2[i]);
  }
}


void compare_cont (const SG::PackedParameters& parms,
                   const std::vector<std::vector<double> >& c1,
                   const std::vector<std::vector<double> >& c2)
{
  assert (c1.size() == c2.size());
  for (size_t i = 0; i < c1.size(); i++) {
    compare_float_vec (parms.nmantissa(), c1[i], c2[i]);
  }
}


template <class T1, class T2>
void compare_cont (const SG::PackedParameters& parms,
                   const std::vector<std::vector<std::vector<T1> > >& c1,
                   const std::vector<std::vector<std::vector<T2> > >& c2)
{
  assert (c1.size() == c2.size());
  for (size_t i = 0; i < c1.size(); i++) {
    compare_cont (parms, c1[i], c2[i]);
  }
}


template <class T1, class T2>
void testit (uint32_t& seed,
             size_t nbits, size_t n = 100, bool is_signed = false,
             size_t nmantissa = 23)
{
  std::vector<T1> c1;

  fill_cont (seed, nbits, c1, n, is_signed);
  assert (c1.size() == n);

  SG::PackedParameters parms;
  parms.setNbits (nbits);
  parms.setNmantissa (nmantissa);
  parms.setSigned (is_signed);
  if (typeid(T1) == typeid(float) ||
      typeid(T1) == typeid(double) ||
      typeid(T1) == typeid(std::vector<float>) ||
      typeid(T1) == typeid(std::vector<double>) ||
      typeid(T1) == typeid(std::vector<std::vector<float> >) ||
      typeid(T1) == typeid(std::vector<std::vector<double> >))
    parms.setFloat (true);

  OutputArgs args;

  SG::PackedConverter cnv (parms);
  cnv.write (n, c1, args);

  std::vector<T2> c2;
  cnv.read (n, c2, args);
  assert (c2.size() == n);

  compare_cont (parms, c1, c2);
}


#define TESTIT1(T) testit<T,T>


void test1 (uint32_t& seed)
{
  std::cout << "test1\n";

  for (int nbits=1; nbits <= 32; ++nbits) {
    TESTIT1(uint32_t) (seed, nbits);
    TESTIT1(uint16_t) (seed, nbits);
    TESTIT1(uint8_t) (seed, nbits);
  }

  for (int nbits=2; nbits <= 32; ++nbits) {
    TESTIT1(int32_t) (seed, nbits, 100, true);
    TESTIT1(int16_t) (seed, nbits, 100, true);
    TESTIT1(int8_t) (seed, nbits, 100, true);
  }

  TESTIT1(float) (seed, 24, 100, false, 16);
  TESTIT1(float) (seed, 24, 100, true, 16);

  TESTIT1(double) (seed, 24, 100, false, 16);
  TESTIT1(double) (seed, 24, 100, true, 16);
}


// Test doubly-nested vectors.
void test2 (uint32_t& seed)
{
  std::cout << "test2\n";
  for (int nbits=1; nbits <= 32; ++nbits) {
    TESTIT1(std::vector<uint32_t>) (seed, nbits);
    TESTIT1(std::vector<uint16_t>) (seed, nbits);
    TESTIT1(std::vector<uint8_t>) (seed, nbits);
  }

  for (int nbits=2; nbits <= 32; ++nbits) {
    TESTIT1(std::vector<int32_t>) (seed, nbits, 100, true);
    TESTIT1(std::vector<int16_t>) (seed, nbits, 100, true);
    TESTIT1(std::vector<int8_t>) (seed, nbits, 100, true);
  }

  TESTIT1(std::vector<float>) (seed, 24, 100, false, 16);
  TESTIT1(std::vector<float>) (seed, 24, 100, true, 16);
  TESTIT1(std::vector<double>) (seed, 24, 100, false, 16);
  TESTIT1(std::vector<double>) (seed, 24, 100, true, 16);
}


// Test triply-nested vectors.
void test3 (uint32_t& seed)
{
  std::cout << "test3\n";
  
  for (int nbits=1; nbits <= 32; ++nbits) {
    TESTIT1(std::vector<std::vector<uint32_t> >) (seed, nbits);
    TESTIT1(std::vector<std::vector<uint16_t> >) (seed, nbits);
    TESTIT1(std::vector<std::vector<uint8_t> >) (seed, nbits);
  }

  for (int nbits=2; nbits <= 32; ++nbits) {
    TESTIT1(std::vector<std::vector<int32_t> >) (seed, nbits, 100, true);
    TESTIT1(std::vector<std::vector<int16_t> >) (seed, nbits, 100, true);
    TESTIT1(std::vector<std::vector<int8_t> >) (seed, nbits, 100, true);
  }

  TESTIT1(std::vector<std::vector<float> >) (seed, 24, 100, false, 16);
  TESTIT1(std::vector<std::vector<float> >) (seed, 24, 100, true, 16);
  TESTIT1(std::vector<std::vector<double> >) (seed, 24, 100, false, 16);
  TESTIT1(std::vector<std::vector<double> >) (seed, 24, 100, true, 16);
}


// Test some simple conversions.
void test4 (uint32_t& seed)
{
  std::cout << "test4\n";

  testit<uint16_t, uint32_t> (seed, 13);
}


int main()
{
  uint32_t seed = 1;
  test1 (seed);
  test2 (seed);
  test3 (seed);
  test4 (seed);
  return 0;
}
