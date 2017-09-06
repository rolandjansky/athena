/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/tests/pointer_list.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009, from earlier code.
 * @brief Unit test for pointer_list.
 */

#undef NDEBUG


#include "CxxUtils/pointer_list.h"
#include <vector>
#include <cassert>
#include <cstdlib>


using namespace CxxUtils;


const size_t toerase[] = {
  0, 1, 5, 14, 15, 16, 17, 20, 29, 30, 31, 500
};
const size_t n_toerase = sizeof (toerase) / sizeof (toerase[0]);


void testerase (pointer_list<>& l,
                size_t nerase,
                std::vector<int>& v)
{
  if (nerase >= n_toerase) std::abort();

  size_t sz = l.size();
  pointer_list<>::iterator it = l.begin();
  std::advance (it, toerase[nerase]);
  l.erase (it);
  assert (l.size() == sz-1);

  it = l.begin();
  pointer_list<>::iterator end = l.end();
  size_t i = 0;
  while (it != end) {
    bool skip = false;
    for (size_t j=0; j <= nerase; j++) {
      if (i == toerase[j]+j) {
        skip = true;
        break;
      }
    }
    if (!skip) {
      assert (*it == &v[i]);
      ++it;
    }
    ++i;
  }

  assert (it == end);
}


void testit1 (pointer_list<>& l, size_t n)
{
  assert (l.size() == 0);
  assert (l.empty());

  std::vector<int> v (n);
  for (size_t i = 0; i < n; i++) {
    l.push_back (&v[i]);
    assert (l.size() == i+1);
  }

  pointer_list<>::iterator beg = l.begin();
  pointer_list<>::iterator end = l.end();
  size_t i = 0;
  while (beg != end) {
    assert (*beg == &v[i]);
    ++beg;
    ++i;
  }
  assert (beg == end);
  assert (i == n);
  if (n > 0)
    assert (!l.empty());

  if (n > 2) {
    beg = l.begin();
    assert (*++beg == &v[1]);
    assert (*beg == &v[1]);
    beg = l.begin();
    assert (*beg++ == &v[0]);
    assert (*beg == &v[1]);
  }

  for (size_t i = 0; i < n_toerase; i++) {
    if (toerase[i] >= l.size())
      break;
    testerase (l, i, v);
  }
}


void testit (int n, pointer_list<>::pool_type& pool)
{
  pointer_list<> l (pool);
  testit1 (l, n);

  // Check that if we clear and refill, we don't allocate more memory.
  size_t inuse = pool.nchunks();
  l.clear();
  testit1 (l, n);
  assert (pool.nchunks() == inuse);
}


void test1()
{
  pointer_list<>::pool_type pool;

  testit (0, pool);
  testit (1, pool);
  testit (5, pool);
  testit (14, pool);
  testit (15, pool);
  testit (16, pool);
  testit (17, pool);
  testit (29, pool);
  testit (30, pool);
  testit (31, pool);
  testit (1000, pool);
}


void test2()
{
  pointer_list<>::pool_type pool;
  std::vector<int> v (35);
  pointer_list<> l (pool);

  for (size_t i = 0; i < 16; i++)
    l.push_back (&v[i]);
  pointer_list<>::iterator pos = l.begin();
  std::advance (pos, 15);
  l.erase (pos);
  pos = l.begin();
  std::advance (pos, 14);
  l.erase (pos);
  l.push_back (&v[14]);

  pos = l.begin();
  pointer_list<>::iterator end = l.end();
  size_t j = 0;
  while (pos != end) {
    assert (*pos == &v[j]);
    ++pos;
    ++j;
  }
}


int main()
{
  test1();
  test2();
  return 0;
}
