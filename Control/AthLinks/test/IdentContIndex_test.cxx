/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthLinks/test/IdentContIndex_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2014
 * @brief Regression tests for IdentContIndex.
 */


#undef NDEBUG
#include "AthLinks/tools/IdentContIndex.h"
#include <vector>
#include <iostream>
#include <cassert>


void test1()
{
  std::cout << "test1\n";

  IdentContIndex i1;
  assert (!i1.isValid());
  assert (i1.collHash() == 0xffff);
  assert (i1.objIndex() == 0xffff);
  assert (i1.hashAndIndex() == 0xffffffff);

  IdentContIndex i2 (123, 456);
  assert (i2.isValid());
  assert (i2.collHash() == 123);
  assert (i2.objIndex() == 456);
  assert (i2.hashAndIndex() == 123*0x10000 + 456);

  IdentContIndex i3 (456*0x10000 + 123);
  assert (i3.isValid());
  assert (i3.collHash() == 456);
  assert (i3.objIndex() == 123);
  assert (i3.hashAndIndex() == 456*0x10000 + 123);

  IdentContIndex i4;
  i4.setCollHash (234);
  assert (!i4.isValid());
  assert (i4.collHash() == 234);
  assert (i4.objIndex() == 0xffff);
  assert (i4.hashAndIndex() == 234*0x10000 + 0xffff);
  i4.setObjIndex (345);
  assert (i4.isValid());
  assert (i4.collHash() == 234);
  assert (i4.objIndex() == 345);
  assert (i4.hashAndIndex() == 234*0x10000 + 345);

  IdentContIndex i5;
  i5.setObjIndex (456);
  assert (!i5.isValid());
  assert (i5.collHash() == 0xffff);
  assert (i5.objIndex() == 456);
  assert (i5.hashAndIndex() == 0xffff0000 + 456);
  i5.setCollHash (567);
  assert (i5.isValid());
  assert (i5.collHash() == 567);
  assert (i5.objIndex() == 456);
  assert (i5.hashAndIndex() == 567*0x10000 + 456);

  i5.setHashAndIndex (678*0x10000 + 789);
  assert (i5.isValid());
  assert (i5.collHash() == 678);
  assert (i5.objIndex() == 789);
  assert (i5.hashAndIndex() == 678*0x10000 + 789);
}


int main()
{
  test1();
  return 0;
}
