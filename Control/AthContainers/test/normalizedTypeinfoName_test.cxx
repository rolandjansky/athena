/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/normalizedTypeinfoName_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2014
 * @brief Regression tests for normalizedTypeinfoName
 */

#undef NDEBUG
#include "AthContainers/normalizedTypeinfoName.h"
#include "AthContainers/DataVector.h"
#include <vector>
#include <map>
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";

  const std::type_info& ti1 = typeid (std::vector<int>);
  assert (SG::normalizedTypeinfoName (ti1) == "std::vector<int>");
  assert (SG::normalizedTypeinfoName (ti1) == "std::vector<int>");

  const std::type_info& ti2 = typeid (DataVector<int>);
  assert (SG::normalizedTypeinfoName (ti2) == "DataVector<int>");

  const std::type_info& ti3 = typeid (std::map<int, float>);
  assert (SG::normalizedTypeinfoName (ti3) == "std::map<int,float>");

  const std::type_info& ti4 = typeid (std::vector<std::string>);
  assert (SG::normalizedTypeinfoName (ti4) == "std::vector<std::string>");

  const std::type_info& ti5 = typeid (std::map<std::string,int>);
  assert (SG::normalizedTypeinfoName (ti5) == "std::map<std::string,int>");
  const std::type_info& ti6 = typeid (std::map<std::string,std::string>);
  assert (SG::normalizedTypeinfoName (ti6) ==
          "std::map<std::string,std::string>");
}


int main()
{
  test1();
  return 0;
}
