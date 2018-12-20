/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file TileByteStream/test/TileRawChannel2Bytes4_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Tests for TileRawChannel2Bytes4.
 */

#undef NDEBUG
#include "TileByteStream/TileRawChannel2Bytes4.h"
#include "TileEvent/TileFastRawChannel.h" 
#include <cassert>
#include <iostream>


void test1()
{
  std::cout << "test1\n";
  TileRawChannel2Bytes4 rch;
  TileFastRawChannel fch;
  fch.set (0, 0, 0, 120.5, 1.25, 5);
  unsigned int w = rch.getWord (fch, 0, 0);
  assert (w == (2440<<16) + (1044<<5) + 5);
  assert (rch.gain(w) == 0);
  assert (rch.amplitude(w, 0) == 120.5);
  assert (rch.time(w) == 1.25);
  assert (rch.quality(w) == 5);

  std::vector<unsigned int> v;
  fch.set (0, 0, 0, -0.5, 1.25, 3);
  assert (rch.getBytes (fch, 1, 1, v) == 1);
  assert (v.size() == 1);
  assert (v[0] == (1024u<<16) + (1044u<<5) + 3u + (1u<<31));
  assert (rch.gain(v[0]) == 1);
  assert (rch.amplitude(v[0], 1) == -0.5);
  assert (rch.time(v[0]) == 1.25);
  assert (rch.quality(v[0]) == 3);
}


int main()
{
  std::cout << "TileRawChannel2Bytes4_test\n";
  test1();
  return 0;
}

