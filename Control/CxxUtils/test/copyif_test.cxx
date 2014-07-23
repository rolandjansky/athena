/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: copyif_test.cxx,v 1.1 2009-03-11 11:06:46 binet Exp $
/**
 * @file CxxUtils/test/copyif_test.cxx
 * @author Sebastien Binet
 * @date March 2009
 * @brief Regression tests for copy_if
 */


#undef NDEBUG


#include "CxxUtils/algorithms.h"
#include <cassert>
#include <vector>
#include <algorithm>

using namespace std;

namespace {
  const std::size_t IMAX = 10;
}

template<typename T>
struct greater_than
{
  T cut;
  greater_than(T cut) : cut(cut) {}

  bool
  operator()(T x) { return x>cut; }
};

void test1()
{
  std::vector<std::size_t> in(IMAX);
  for (std::size_t i = 0; i!=IMAX; ++i){
    in[i] = i;
  }
  for (std::size_t i = 0; i!=IMAX; ++i){
    assert(in[i] == i);
  }

  std::vector<std::size_t> out;
  CxxUtils::copy_if(in.begin(), in.end(),
		    std::back_inserter(out),
		    greater_than<std::size_t>(4));
  assert(out.size()==5);
  assert(out[0]==5);
  assert(out[1]==6);
  assert(out[2]==7);
  assert(out[3]==8);
  assert(out[4]==9);
}

void test2()
{
  const std::size_t in[IMAX] = {0, 1,2,3,4,5,6,7,8,9};
  for (std::size_t i = 0; i!=IMAX; ++i){
    assert(in[i] == i);
  }

  std::vector<std::size_t> out;
  CxxUtils::copy_if(in, in+IMAX,
		    std::back_inserter(out),
		    greater_than<std::size_t>(4));
  assert(out.size()==5);
  assert(out[0]==5);
  assert(out[1]==6);
  assert(out[2]==7);
  assert(out[3]==8);
  assert(out[4]==9);
}


int main()
{
  test1();
  test2();
  return 0;
}
