/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAccess/test/DVCollectionProxy_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2014
 * @brief Regression tests for DVCollectionProxy.
 */


#undef NDEBUG


#include "AthenaROOTAccess/DVCollectionProxy.h"
#include "AthenaROOTAccess/TestTypes.h"
#include "TInterpreter.h"
#include <iostream>
#include <cassert>


template <class DV>
void do_test_1 (const char* elttype, const char* conttype)
{
  typedef typename DV::base_value_type ELT;

  AthenaROOTAccess::DVCollectionProxy prox (elttype, conttype);
  DV dv;
  dv.push_back (new ELT (1));
  dv.push_back (new ELT (2));
  dv.push_back (new ELT (3));
  dv.push_back (new ELT (4));
  dv.push_back (new ELT (5));

  prox.PushProxy (&dv);
  assert (prox.Size() == 5);

  ELT** elt = reinterpret_cast<ELT**> (prox.At(0));
  assert ((*elt)->x == 1);
  elt = reinterpret_cast<ELT**> (prox.At(1));
  assert ((*elt)->x == 2);
  elt = reinterpret_cast<ELT**> (prox.At(2));
  assert ((*elt)->x == 3);

  prox.Clear();
  assert (dv.size() == 0);
}


void test1()
{
  std::cout << "test1\n";
  do_test_1<ARATest::AVec> ("ARATest::A*", "DataVector<ARATest::A>");
  do_test_1<ARATest::CVec> ("ARATest::C*", "DataVector<ARATest::C>");
  do_test_1<ARATest::DVec> ("ARATest::D*", "DataVector<ARATest::D>");
  do_test_1<ARATest::EVec> ("ARATest::E*", "DataVector<ARATest::E>");
  do_test_1<ARATest::FVec> ("ARATest::F*", "DataVector<ARATest::F>");
  do_test_1<ARATest::GVec> ("ARATest::G*", "DataVector<ARATest::G>");
}


int main()
{
  gInterpreter->EnableAutoLoading();

  TClass::GetClass("DataVector<AthenaROOTAccess::DVCollectionProxyDummy>");
  test1();

  // root6 will crash if a *Dict.so file gets unloaded before libCling.
  // However, that's sometimes happening here for some reason.
  // For now, just take a big hammer to it and exit without
  // running global dtors.
  std::cout.flush();
  std::cerr.flush();
  _exit(0);
  return 0;
}
