/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaPoolCnvSvc/test/exceptions_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for exceptions.
 */


#undef NDEBUG
#include "AthenaPoolCnvSvc/exceptions.h"
#include <iostream>
#include <typeinfo>


void test1()
{
  std::cout << "test1\n";

  std::cout << AthenaPoolCnvSvc::ExcNoDictForClass (typeid(int)).what() << "\n";
  std::cout << AthenaPoolCnvSvc::ExcUnsupportedVersion
    ( typeid(int),
      Guid("336F636C-D414-4261-8286-37429F353F0A") ).what() << "\n";
}


int main()
{
  test1();
  return 0;
}
