/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PackedArray_test.cxx,v 1.2 2008-12-12 04:26:20 ssnyder Exp $
/**
 * @file CxxUtils/test/PackedArray_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2007
 * @brief Regression test for PackedArray class.
 */

#undef NDEBUG

#include "CxxUtils/PackedArray.h"
#include <cassert>
#include <vector>
#include <stdexcept>
#include <climits>
#include <cstdlib>


using CxxUtils::PackedArray;


void compare (const PackedArray& arr, const std::vector<int>& v)
{
  unsigned int bitmask = (1<<arr.bitsize()) - 1;
  assert (arr.size() == v.size());
  for (size_t i = 0; i < arr.size(); i++)
    assert (arr[i] == (v[i] & bitmask));
}


void testit (PackedArray& arr)
{
  int bitsize = arr.bitsize();
  unsigned int bitmask = (1<<bitsize) - 1;

  std::vector<int> v;

  assert (arr.max_size() == v.max_size());

  const PackedArray& carr = arr;
  const std::vector<int>& cv = v;
  assert (arr.empty());
  arr.assign (10, 17);
  v.assign (10, 17);
  compare (arr, v);
  assert (!arr.empty());

  arr.reserve (100);
  int nper = sizeof (unsigned int) * CHAR_BIT;
  assert (arr.capacity() ==
          (size_t)(((100 * bitsize) + nper-1) / nper) * nper / bitsize);

  unsigned int x;
  for (int i=0; i < 100; i++) {
    x = std::rand();
    v.push_back (x);
    arr.push_back (x);
  }
  compare (arr, v);

  for (int i=0; i < 20; i++) {
    v.pop_back();
    arr.pop_back();
  }
  compare (arr, v);

  // cppcheck-suppress invalidContainerReference // false positive
  assert (carr.front() == (cv.front() & bitmask));
  assert (arr.front() == (v.front() & bitmask));
  x = std::rand();
  arr.front() = x;
  v.front() = x;
  assert (carr.front() == (cv.front() & bitmask));
  assert (arr.front() == (v.front() & bitmask));
  compare (arr, v);

  assert (carr.back() == (cv.back() & bitmask));
  assert (arr.back() == (v.back() & bitmask));
  x = std::rand();
  arr.back() = x;
  v.back() = x;
  assert (carr.back() == (cv.back() & bitmask));
  assert (arr.back() == (v.back() & bitmask));
  compare (arr, v);

  for (int i=0; i < 20; i++) {
    int j = std::rand() % arr.size();
    assert (carr[j] == (cv[j] & bitmask));
    assert (arr[j] == (v[j] & bitmask));
    x = std::rand();
    arr[j] = x;
    v[j] = x;
    assert (carr[j] == (cv[j] & bitmask));
    assert (arr[j] == (v[j] & bitmask));
  }
  compare (arr, v);

  for (int i=0; i < 20; i++) {
    int j = std::rand() % arr.size();
    assert (carr.at(j) == (cv.at(j) & bitmask));
    assert (arr.at(j) == (v.at(j) & bitmask));
    x = std::rand();
    arr.at(j) = x;
    v.at(j) = x;
    assert (carr.at(j) == (cv.at(j) & bitmask));
    assert (arr.at(j) == (v.at(j) & bitmask));
  }
  compare (arr, v);

  bool caught = false;
  try {
    arr.at (100000);
  }
  catch (const std::out_of_range& e)
  {
    caught = true;
  }
  assert (caught);

  caught = false;
  try {
    carr.at (100000);
  }
  catch (const std::out_of_range& e)
  {
    caught = true;
  }
  assert (caught);

  for (int i=0; i < 20; i++) {
    int j = std::rand() % arr.size();
    assert (arr.get(j) == (v[j] & bitmask));
    x = std::rand();
    arr.set (j, x);
    v[j] = x;
    assert (arr.get(j) == (v[j] & bitmask));
  }
  compare (arr, v);

  for (int i=0; i < 20; i++) {
    int sz = arr.size();
    int inc  = std::rand()%100 + 5;
    arr.resize (sz + inc);
    v.resize (sz + inc);
    compare (arr, v);
    assert (arr[sz + inc/2] == 0);
  }
  {
    int sz = arr.size();
    arr.resize (sz-31);
    v.resize (sz-31);
    compare (arr, v);
  }
  for (int i=0; i < 20; i++) {
    int sz = arr.size();
    int inc  = std::rand()%100 + 5;
    x = std::rand();
    arr.resize (sz + inc, x);
    v.resize (sz + inc, x);
    compare (arr, v);
    assert (arr[sz + inc/2] == (x&bitmask));
  }

  arr.assign (10, 0);
  v.assign (10, 0);
  compare (arr, v);

  PackedArray arr2 (bitsize, 10, 0, arr.get_allocator());
  assert (arr2.size() == 10);
  for (int i=0; i < 10; i++)
    assert (arr2[i] == 0);

  x = std::rand();
  PackedArray arr3 (bitsize, 10, x);
  assert (arr3.size() == 10);
  for (int i=0; i < 10; i++)
    assert (arr3[i] == (x & bitmask));

  arr.swap (arr3);
  assert (arr.size() == 10);
  for (int i=0; i < 10; i++)
    assert (arr[i] == (x & bitmask));
  compare (arr3, v);

  arr.clear();
  assert (arr.empty());
  assert (arr.size() == 0);
}


void test1()
{
  PackedArray arr (10);
  testit (arr);
  arr.set_bitsize (20);
  testit (arr);
  arr.set_bitsize (3);
  testit (arr);
}


int main()
{
  test1();
  return 0;
}
