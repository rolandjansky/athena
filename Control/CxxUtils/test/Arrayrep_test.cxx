/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Arrayrep_test.cxx,v 1.2 2009-04-08 21:12:45 ssnyder Exp $
/**
 * @file  Arrayrep_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date June, 2004
 * @brief Regression tests for the Arrayrep class.
 */

#undef NDEBUG

#include "CxxUtils/Arrayrep.h"
#include <stdexcept>
#include <cassert>


using namespace CxxUtils;


const unsigned int shape[] = {2, 4, 3};
const Arrayelt data[] = { 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
                          13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};


void test1()
{
  Arrayrep ar0 (" 3.5 ");
  assert (ar0.m_shape.size() == 0);
  assert (ar0.m_data.size() == 1);
  assert (ar0.m_data[0] == 3.5);

#define EXPECT_EXCEPTION(s) \
  { \
    bool caught = false; \
    try { \
      Arrayrep xx (s); \
    } catch (std::runtime_error& c) {           \
      caught = true; \
    } \
    assert (caught); \
  }

  EXPECT_EXCEPTION ("  asd ");
  EXPECT_EXCEPTION ("  3.5 3.5 ");

  Arrayrep ar1 (" [[ 1.5, 2.5], [3.5, 4.5]] ");
  assert (ar1.m_shape.size() == 2);
  assert (ar1.m_shape[0] == 2);
  assert (ar1.m_shape[1] == 2);
  assert (ar1.m_data.size() == 4);
  assert (ar1.m_data[0] == 1.5);
  assert (ar1.m_data[1] == 2.5);
  assert (ar1.m_data[2] == 3.5);
  assert (ar1.m_data[3] == 4.5);

  EXPECT_EXCEPTION (" [[ 1.5, 2.5], [3.5, 4.5, 5.5]] ");
  EXPECT_EXCEPTION (" [[ 1.5, 2.5], [3.5, 4.5], [[6.5, 7.5]]]] ");
  EXPECT_EXCEPTION (" [[ 1.5, 2.5], [4.5]] ");
  EXPECT_EXCEPTION (" [[ 1.5, 2.5], [asd]] ");
  EXPECT_EXCEPTION (" [[ 1.5, 2.5], [3.5, 4.5] ");
}


void test2()
{
  Arrayrep rep;
  rep.init_sizes();
  rep.m_shape.assign (std::begin(shape), std::end(shape));
  rep.m_data.assign (std::begin(data), std::end(data));
  rep.init_sizes();
  assert (rep.m_sizes.size() == 3);
  assert (rep.m_sizes[0] == 1);
  assert (rep.m_sizes[1] == 3);
  assert (rep.m_sizes[2] == 12);
}


void test3()
{
  Arrayrep rep1 (shape, std::end(shape)-std::begin(shape));
  assert (rep1.m_shape.size() == 3);
  assert (rep1.m_shape[0] == 2);
  assert (rep1.m_shape[1] == 4);
  assert (rep1.m_shape[2] == 3);
  assert (rep1.m_data.size() == 24);

  std::vector<unsigned int> v;
  v.push_back (4);
  v.push_back (2);
  Arrayrep rep2 (v);
  assert (rep2.m_shape.size() == 2);
  assert (rep2.m_shape[0] == 4);
  assert (rep2.m_shape[1] == 2);
  assert (rep2.m_data.size() == 8);
}


int main()
{
  test1();
  test2();
  test3();
  return 0;
}
