/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAccess/test/DataBucketVoid_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2014
 * @brief Regression tests for DataBucketVoid.
 */


#undef NDEBUG


#include "AthenaROOTAccess/DataBucketVoid.h"
#include "AthenaROOTAccess/TestTypes.h"
#include "TInterpreter.h"
#include "TClass.h"
#include <iostream>
#include <cassert>


template <class T>
int getval (const T* p)
{
  return p->x;
}


int getval (const ARATest::H* p)
{
  return p->h - 40;
}


template <class DV, class DVBASE>
void do_test1 (const char* clsname)
{
  typedef typename DV::base_value_type ELT;

  DV dv;
  dv.push_back (new ELT (1));
  dv.push_back (new ELT (2));
  dv.push_back (new ELT (3));
  dv.push_back (new ELT (4));
  dv.push_back (new ELT (5));

  TClass* cls = TClass::GetClass (clsname);
  AthenaROOTAccess::DataBucketVoid b (cls, &dv);

  const DVBASE* dvbase = reinterpret_cast<DVBASE*> (b.cast (typeid(DVBASE)));
  assert (dvbase->size() == 5);
  assert (getval((*dvbase)[3]) == 4);

  b.reset();
}


void test1()
{
  std::cout << "test1\n";

  do_test1<ARATest::AVec, ARATest::AVec> ("DataVector<ARATest::A>");
  do_test1<ARATest::CVec, ARATest::AVec> ("DataVector<ARATest::C>");
  do_test1<ARATest::EVec, ARATest::DVec> ("DataVector<ARATest::E>");
  do_test1<ARATest::GVec, ARATest::DVec> ("DataVector<ARATest::G>");
  do_test1<ARATest::GVec, ARATest::HVec> ("DataVector<ARATest::G>");

  do_test1<ARATest::EDer, ARATest::DVec> ("ARATest::EDer");
}


int main()
{
  gInterpreter->EnableAutoLoading();
  test1();
  return 0;
}
  
