/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/test/TopBase_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2018
 * @brief Unit test for TopBase.
 */


#undef NDEBUG
#include "AthenaKernel/TopBase.h"
#include <iostream>
#include <cassert>


class C1 {};
class C2 : public C1 {};
class C3 : public C2 {};


SG_BASES (C2, C1);
SG_BASES (C3, C2);


void test1()
{
  std::cout << "test1\n";

  assert ((std::is_same<SG::TopBase<C1>::type, C1>::value));
  assert ((std::is_same<SG::TopBase<C2>::type, C1>::value));
  assert ((std::is_same<SG::TopBase<C3>::type, C1>::value));
}


int main()
{
  std::cout << "TopBase_test\n";
  test1();
  return 0;
}
