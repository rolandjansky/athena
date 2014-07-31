/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file vectorize_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Regression tests for the vectorize class.
 */

#undef NDEBUG

#include "EventTPCnv/vectorize.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <cassert>

using std::rand;
using std::isprint;


void test_string()
{
  for (int len=0; len < 8192; len++) {
    std::string s;
    s.reserve (len);
    for (int i=0; i < len; i++) {
      char c;
      do {
        c = rand() % 256;
      } while (!isprint(c) && c != '\n' && c != '\r' && c != '\t');
      s.push_back (c);
    }

    std::vector<unsigned int> v;
    strToUI (s, v);
    std::string s2;
    std::vector<unsigned int>::const_iterator it = v.begin();
    UITostr (s2, it, false);
    assert (s == s2);
    assert (it == v.end());
  }
}


void test_string_bug()
{
  for (int len=0; len < 8192; len++) {
    if (len == 2) continue;

    std::string s;
    s.reserve (len);
    for (int i=0; i < len; i++) {
      char c;
      do {
        c = rand() % 256;
      } while (!isprint(c) && c != '\n' && c != '\r' && c != '\t');
      s.push_back (c);
    }

    std::vector<unsigned int> v;
    strToUI (s, v);
    if ((len%4) == 2) {
      v[v.size()-1] = 0x01020304;
      s.resize (len-4);
    }

    std::string s2;
    std::vector<unsigned int>::const_iterator it = v.begin();
    UITostr (s2, it, true);
    assert (s == s2);
    if ((len%4) == 2)
      assert (it == v.end()-1);
    else
      assert (it == v.end());
  }
}


void test_bitmap()
{
  for (int len=0; len < 8192; len++) {
    std::vector<bool> b;
    b.reserve (len);
    for (int i=0; i < len; i++) {
      b.push_back (rand() & 1);
    }

    std::vector<unsigned int> v;
    bitmapToUI (b, v);
    std::vector<bool> b2;
    std::vector<unsigned int>::const_iterator it = v.begin();
    UITobitmap (b2, it, false);
    assert (b == b2);
    assert (it == v.end());
  }
}


void test_bitmap_bug()
{
  for (int len=0; len < 8192; len++) {
    if (len == 16) continue;
    std::vector<bool> b;
    b.reserve (len);
    for (int i=0; i < len; i++) {
      b.push_back (rand() & 1);
    }

    std::vector<unsigned int> v;
    bitmapToUI (b, v);
    if ((len%32) == 16) {
      v.resize (v.size()-1);
      b.resize (len-32);
    }

    std::vector<bool> b2;
    std::vector<unsigned int>::const_iterator it = v.begin();
    UITobitmap (b2, it, true);
    assert (b == b2);
    assert (it == v.end());
  }
}


int main()
{
  test_string();
  test_string_bug();
  test_bitmap();
  test_bitmap_bug();
  return 0;
}
