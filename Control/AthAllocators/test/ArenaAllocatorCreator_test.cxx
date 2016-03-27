/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaAllocatorCreator_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaAllocatorCreator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaAllocatorCreator.
 */

#undef NDEBUG
#include "AthAllocators/ArenaAllocatorCreator.h"
#include <cassert>

class Test
  : public SG::ArenaAllocatorCreator
{
public:
  virtual SG::ArenaAllocatorBase* create() { return 0; }
};
  

int main()
{
  Test test;
  assert (test.create() == 0);
  return 0;
}
