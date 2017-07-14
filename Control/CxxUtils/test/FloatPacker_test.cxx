/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/test/FloatPacker_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009, from earlier code.
 * @brief Regression tests for FloatPacker.
 */

#ifndef __APPLE__

#undef NDEBUG

#include "CxxUtils/FloatPacker.h"
#include <iostream>
#include <cmath>
#include <cassert>
#include <ieee754.h>
#include <fenv.h>
#include <string.h>


using CxxUtils::FloatPacker;
using std::cout;
using std::abs;


const int ieee754_double_exponent_bits = 11;


bool bitwise_equal (double a, double b)
{
  return (memcmp (&a, &b, sizeof (a)) == 0);
}


bool almost_equal (double a, double b, int bits)
{
  return (abs ((a-b)/(a+b))) * (1<<bits) < 1;
}


void test1 ()
{
  // Set up some special numbers for testing.
  ieee754_double d;
  d.d = 0;
  d.ieee.negative = 1;
  double neg_zero = d.d;

  d.d = 0;
  d.ieee.exponent = (1<<ieee754_double_exponent_bits) - 1;
  double infinity = d.d;

#ifndef __alpha
  // Can we handle denormals?
  bool have_denormal = false;
#ifdef __FLT_HAS_DENORM__
  have_denormal = __FLT_HAS_DENORM__;
#endif
#ifdef __x86_64__
  {
    fenv_t fenv;
    fegetenv(&fenv);
    if (fenv.__mxcsr & (1<<6)) // Test DAZ flag.
      have_denormal = false;
  }
#endif

  const double denormal = 3e-320;
  const double denormal2 = 3072e-320;
#endif

  FloatPacker tf1 (12, 12);

  double unpacked;
  FloatPacker::Packdest packed;
  double out;
  std::string err;

  unpacked = 0.125;
  packed = tf1.pack (unpacked);
  assert (packed == 0x100);
  out = tf1.unpack (packed);
  assert (bitwise_equal (unpacked, out));

  unpacked = 0;
  packed = tf1.pack (unpacked);
  assert (packed == 0);
  out = tf1.unpack (packed);
  assert (bitwise_equal (unpacked, out));

  unpacked = -0.125;
  packed = tf1.pack (unpacked);
  assert (packed == 0x900);
  out = tf1.unpack (packed);
  assert (bitwise_equal (unpacked, out));

  unpacked = neg_zero;
  packed = tf1.pack (unpacked);
  assert (packed == 0x800);
  out = tf1.unpack (packed, &err);
  assert (bitwise_equal (unpacked, out));
  assert (err.empty());

  FloatPacker tf2 (28, 24, 2, false);

  unpacked = 7.5;
  packed = tf2.pack (unpacked);
  assert (packed == 0x9e00000);
  out = tf2.unpack (packed);
  assert (bitwise_equal (unpacked, out));

  unpacked = 7.5 + (0.5)/2/16/16/16/16/16;
  packed = tf2.pack (unpacked);
  assert (packed == 0x9e00001);
  out = tf2.unpack (packed);
  assert (bitwise_equal (unpacked, out));

  unpacked = neg_zero;
  packed = tf2.pack (unpacked);
  assert (packed == 0);
  out = tf2.unpack (packed);
  assert (out == 0);

  unpacked = 0;
  packed = tf2.pack (unpacked);
  assert (packed == 0);
  out = tf2.unpack (packed, &err);
  assert (bitwise_equal (unpacked, out));
  assert (err.empty());

  // This gets a warning.
  unpacked = -1;
  packed = tf2.pack (unpacked, &err);
  assert (packed == 0);
  out = tf2.unpack (packed, &err);
  assert (bitwise_equal (0, out));
  assert (err == "Float overflow during packing: -1");
  err.clear();

  // This gets a warning.
  unpacked = infinity;
  packed = tf2.pack (unpacked, &err);
  assert (packed == 0);
  out = tf2.unpack (packed, &err);
  assert (bitwise_equal (0, out));
  assert (err == "Bad float number: inf (0 7ff00000)");
  err.clear();

  // This gets a warning.
  unpacked = 100000;
  packed = tf2.pack (unpacked, &err);
  assert (packed == 0xfffffff);
  out = tf2.unpack (packed, &err);
  assert (almost_equal (out, 512, 24));
  assert (err == "Float overflow during packing: 100000");
  err.clear();

  // Testing underflow to denormal.
  unpacked = 0.5 / 32;
  packed = tf2.pack (unpacked);
  assert (packed == 0x1000000);
  out = tf2.unpack (packed);
  assert (bitwise_equal (unpacked, out));

  unpacked /= 2;
  packed = tf2.pack (unpacked);
  assert (packed == 0x0800000);
  out = tf2.unpack (packed);
  assert (bitwise_equal (unpacked, out));

  unpacked /= 2;
  packed = tf2.pack (unpacked);
  assert (packed == 0x0400000);
  out = tf2.unpack (packed);
  assert (bitwise_equal (unpacked, out));

  // Underflow to zero.
  unpacked = 1e-50;
  packed = tf2.pack (unpacked);
  assert (packed == 0);
  out = tf2.unpack (packed, &err);
  assert (bitwise_equal (0, out));
  assert (err.empty());

  FloatPacker tf3 (28, 12, 0, false);

#ifndef __alpha
  // Having problems with denormals on alpha.  Just skip for now.
  unpacked = denormal;
  packed = tf3.pack (unpacked);
  assert (packed == 0x7be1000);
  out = tf3.unpack (packed);
  assert (bitwise_equal (0, out));

  unpacked = denormal2;
  packed = tf3.pack (unpacked);
  assert (packed == 0x7be4400);
  out = tf3.unpack (packed);
  if (have_denormal)
    assert (almost_equal (out, unpacked, 3));
  else
    assert (out == 0);
#endif
  assert (err.empty());

  // Testing rounding.
  FloatPacker tf6 (12, 8, 4, false, true);

  unpacked = 990./1024; // This doesn't get rounded.
  packed = tf6.pack (unpacked);
  assert (packed == 0x5ef);
  out = tf6.unpack (packed);
  assert (bitwise_equal (unpacked, out));

  unpacked = 991./1024; // This does get rounded.
  packed = tf6.pack (unpacked);
  assert (packed == 0x5f0);
  out = tf6.unpack (packed);
  double tmp = 992./1024;
  assert (bitwise_equal (tmp, out));

  unpacked = 1023./1024; // Test mantissa overflowing after rounding.
  packed = tf6.pack (unpacked);
  assert (packed == 0x600);
  out = tf6.unpack (packed, &err);
  tmp = 1.0;
  assert (bitwise_equal (tmp, out));
  assert (err.empty());

  // Test an unpacking problem with npack==packdest_bits.
  FloatPacker tf7 (32, 32, 0, false);
  unpacked = 1. / 65536 / 65536;
  packed = tf7.pack (unpacked, &err);
  assert (packed == 0x1);
  out = tf7.unpack (packed, &err);
  assert (bitwise_equal (unpacked, out));
  assert (err.empty());

  // Test rounding during underflow_to_denormal.
  FloatPacker tf8 (8, 8, 0, false, true);
  unpacked = 0.5 + 1. / 256;
  packed = tf8.pack (unpacked);
  assert (packed == 0x81);
  out = tf8.unpack (packed);
  assert (bitwise_equal (unpacked, out));

  unpacked = 0.5 + 1. / 512;
  packed = tf8.pack (unpacked, &err);
  assert (packed == 0x81);
  out = tf8.unpack (packed, &err);
  tmp = 0.5 + 1. / 256;
  assert (bitwise_equal (tmp, out));
  assert (err.empty());

  // This gets a warning.
  unpacked = 1023. / 1024;
  packed = tf8.pack (unpacked, &err);
  assert (packed == 0xff);
  d.d = unpacked;
  assert (err == "Float overflow during packing: 0.999023");
  err.clear();
  out = tf8.unpack (packed, &err);
  tmp = 255. / 256;
  assert (bitwise_equal (tmp, out));
  assert (err.empty());

  // Test using bit 33 to control rounding.
  FloatPacker tf9 (32, 32, 0, false, true);
  unpacked = 0.5 + 1. / 65536 / 65536;
  packed = tf9.pack (unpacked);
  assert (packed == 0x80000001);
  out = tf9.unpack (packed);
  assert (bitwise_equal (unpacked, out));

  unpacked = 0.5 + 1. / 65536 / 65536 / 2;
  packed = tf9.pack (unpacked);
  assert (packed == 0x80000001);
  out = tf9.unpack (packed, &err);
  tmp = 0.5 + 1. / 65536 / 65536;
  assert (bitwise_equal (tmp, out));
  assert (err.empty());

  // This gets a warning.
  packed = 0xf654000;
  out = tf3.unpack (packed, &err);
  assert (err == "Overflow while unpacking float; exponent: 30292");
  assert (std::isinf (out));
  err.clear();
}


#else


void test1() {}


#endif // not __APPLE__


int main ()
{
  test1 ();
  return 0;
}
