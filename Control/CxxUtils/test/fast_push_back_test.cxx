/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: fast_push_back_test.cxx,v 1.1 2009-03-20 18:59:20 ssnyder Exp $
/**
 * @file  CxxUtils/tests/fast_push_back_test.cxx
 * @author scott snyder
 * @date Jan, 2007
 * @brief Regression tests for fast_push_back.
 */


#undef NDEBUG
#include "CxxUtils/fast_push_back.h"
#include <vector>
#include <cassert>


class C
{
public:
  C () : m_a (0), m_b (0), m_c (0), m_d (0) {}
  C (int a) : m_a (a), m_b (0), m_c (0), m_d (0) {}
  C (int a, int b) : m_a (a), m_b (b), m_c (0), m_d (0) {}
  C (int a, int b, int c) : m_a (a), m_b (b), m_c (c), m_d (0) {}
  C (int a, int b, int c, int d) : m_a (a), m_b (b), m_c (c), m_d (d) {}
  ~C () {}
  int m_a;
  int m_b;
  int m_c;
  int m_d;
};


void check (const C& cc, int a=0, int b=0, int c=0, int d=0)
{
  assert (cc.m_a == a);
  assert (cc.m_b == b);
  assert (cc.m_c == c);
  assert (cc.m_d == d);
}


void testit()
{
  std::vector<C> v;

  for (int i=0; i < 100; i++)
    CxxUtils::fast_push_back (v);
  for (int i=0; i < 100; i++)
    CxxUtils::fast_push_back (v, i);
  for (int i=0; i < 100; i++)
    CxxUtils::fast_push_back (v, i, i*2);
  for (int i=0; i < 100; i++)
    CxxUtils::fast_push_back (v, i, i*2, i*3);
  for (int i=0; i < 100; i++)
    CxxUtils::fast_push_back (v, i, i*2, i*3, i*4);

  int j = 0;
  for (int i=0; i < 100; i++)
    check (v[j++]);
  for (int i=0; i < 100; i++)
    check (v[j++], i);
  for (int i=0; i < 100; i++)
    check (v[j++], i, i*2);
  for (int i=0; i < 100; i++)
    check (v[j++], i, i*2, i*3);
  for (int i=0; i < 100; i++)
    check (v[j++], i, i*2, i*3, i*4);
}


int main()
{
  testit();
  return 0;
}

