/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaKernel/test/DataObjectSharedPtr_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Regression tests for DataObjectSharedPtr.
 */


#undef NDEBUG
#include "AthenaKernel/DataObjectSharedPtr.h"
#include <iostream>


class MyObj : public DataObject
{
public:
  virtual ~MyObj() override { std::cout << "MyObj dtor\n"; }
};


int f (SG::DataObjectSharedPtr<DataObject> ptr)
{
  return ptr->refCount();
}


void test1()
{
  std::cout << "test1\n";
  {
    SG::DataObjectSharedPtr<MyObj> ptr (new MyObj);
    assert (ptr->refCount() == 1);
    {
      SG::DataObjectSharedPtr<MyObj> ptr2 (ptr);
      assert (ptr->refCount() == 2);
    }
    assert (ptr->refCount() == 1);

    assert (f (ptr) == 2);
    assert (ptr->refCount() == 1);
    
    std::cout << "should call dtor now\n";
  }
}


int main()
{
  test1();
  return 0;
}
