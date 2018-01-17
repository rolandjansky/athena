/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/dataVectorAsELV_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief Tests for dataVectorAsELV.
 */


// Disable this test in standalone mode:
#ifndef XAOD_STANDALONE

#undef NDEBUG
#include "AthContainers/dataVectorAsELV.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxElement.h"
#include "SGTools/TestStore.h"
#include "TestTools/expect_exception.h"
#include "CxxUtils/checker_macros.h"
#include <iostream>
#include <cassert>


class X {};
CLASS_DEF( DataVector<X> , 9901 , 1 )

class Y : public SG::AuxElement {};
CLASS_DEF( DataVector<Y> , 9902 , 1 )


void test1()
{
  std::cout << "test1\n";
  size_t N = 3;

  DataVector<X> dvx1;
  for (size_t i = 0; i < N; i++)
    dvx1.push_back (new X);

  std::vector<ElementLink<DataVector<X> > > linksx1 =
    SG::dataVectorAsELV (dvx1);

  for (size_t i = 0; i < N; i++)
    assert (dvx1[i] == *linksx1[i]);

  DataVector<X> dvx2 (SG::VIEW_ELEMENTS);
  for (size_t i = 0; i < N; i++)
    dvx2.push_back (dvx1[N-1-i]);

  EXPECT_EXCEPTION (SG::ExcDVToELV, SG::dataVectorAsELV (dvx2));

  DataVector<Y> dvy1;
  for (size_t i = 0; i < N; i++)
    dvy1.push_back (new Y);

  std::vector<ElementLink<DataVector<Y> > > linksy1 =
    SG::dataVectorAsELV (dvy1);

  for (size_t i = 0; i < N; i++)
    assert (dvy1[i] == *linksy1[i]);

  DataVector<Y> dvy2 (SG::VIEW_ELEMENTS);
  for (size_t i = 0; i < N; i++)
    dvy2.push_back (dvy1[N-1-i]);

  std::vector<ElementLink<DataVector<Y> > > linksy2 =
    SG::dataVectorAsELV (dvy2);

  for (size_t i = 0; i < N; i++)
    assert (dvy2[i] == *linksy2[i]);

  dvy2.push_back (new Y);
  EXPECT_EXCEPTION (SG::ExcDVToELV, SG::dataVectorAsELV (dvy2));
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  SGTest::initTestStore();
  test1();
  return 0;
}

#else

int main() {
   return 0;
}

#endif // not XAOD_STANDALONE
