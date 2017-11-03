/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file CaloDetDescr/test.CaloConstIteratorAdaptor_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2017
 * @brief Unit test for CaloConstIteratorAdaptor.
 */

#undef NDEBUG
#include "CaloDetDescr/CaloConstIteratorAdaptor.h"
#include <vector>
#include <type_traits>
#include <iostream>
#include <cassert>


typedef std::vector<int*> vec_t;


void test1()
{
  std::cout << "test1\n";

  std::vector<int> vv { 0, 1, 2, 3, 4 };
  vec_t vec { &vv[0], &vv[1], &vv[2], &vv[3], &vv[4] };
  typedef CaloConstIteratorAdaptor<vec_t::const_iterator> it_t;

  assert (!std::is_const<std::remove_pointer<vec_t::const_iterator::value_type>::type>::value);
  assert (std::is_const<std::remove_pointer<it_t::value_type>::type>::value);
  it_t it = it_t(vec.begin());
  it_t end = it_t(vec.end());
  int i = 0;
  for (; it < end; ++it) {
    assert (i == **it);
    ++i;
  }
}


int main()
{
  test1();
  return 0;
}
