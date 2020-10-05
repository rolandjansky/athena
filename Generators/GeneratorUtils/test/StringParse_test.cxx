/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Suppress a gcc8 warning from boost.
// (Binaries apparently include boost with -I rather than -isystem.)
// Fixed in boost 1.68 (see https://github.com/boostorg/mpl/issues/31)
#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wparentheses"
#endif
#include "GeneratorUtils/StringParse.h"
#ifdef __GNUC__
# pragma GCC diagnostic pop
#endif
#include <iostream>
#include <cmath>

// Make sure that asserts will work in opt builds
#undef NDEBUG
#include <cassert>


int main() {
  StringParse s("1234 foo 5.678   1234argh 5.678bleurgh");
  assert(s.num_pieces() == 5);

  assert(s.piece(1) == "1234");
  assert(s.piece<int>(1) == 1234);
  assert(s.intpiece(1) == 1234);

  assert(s.piece(2) == "foo");
  assert(s.piece<std::string>(2) == "foo");

  assert(s.piece(3) == "5.678");
  assert(std::abs(s.piece<double>(3) - 5.678) < 1e-6);
  assert(std::abs(s.numpiece(3) - 5.678) < 1e-6);

  assert(s.piece<int>(4) != 1234);
  assert(s.intpiece(4) == -1);

  assert(std::abs(s.piece<double>(5) + 1) < 1e-6);
  assert(std::abs(s.numpiece(5) + 1) < 1e-6);
}
