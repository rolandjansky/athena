/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/test/ILockableTool_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2022
 * @brief Unit test for ILockableTool.
 */


#undef NDEBUG
#include "AthenaKernel/ILockableTool.h"
#include <iostream>
#include <cassert>


class TestTool : public ILockableTool
{
public:
  virtual void lock_shared() const override { std::cout << "lock\n"; }
  virtual void unlock_shared() const override  { std::cout << "unlock\n"; }
};


void test1()
{
  std::cout << "test1\n";
  TestTool tool;
  {
    Athena::ToolLock lock (tool);
  }
  std::cout << "test1 exiting\n";
}


int main()
{
  std::cout << "AthenaKernel/ILockableTool_test\n";
  test1();
  return 0;
}
