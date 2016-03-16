/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigInDetEventTPCnv/test/TrigInDetTrackFitParCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigInDetTrackFitParCnv_p1.
 */


#undef NDEBUG
#include "TrigInDetEventTPCnv/TrigInDetTrackFitParCnv_p1.h"
#include "CxxUtils/make_unique.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const TrigInDetTrackFitPar& p1,
              const TrigInDetTrackFitPar& p2)
{
  assert (p1.a0() == p2.a0());
  assert (p1.phi0() == p2.phi0());
  assert (p1.z0() == p2.z0());
  assert (p1.eta() == p2.eta());
  assert (p1.pT() == p2.pT());
  assert (Athena_test::isEqual (p1.ea0(), p2.ea0()));
  assert (Athena_test::isEqual (p1.ephi0(), p2.ephi0()));
  assert (Athena_test::isEqual (p1.ez0(), p2.ez0()));
  assert (Athena_test::isEqual (p1.eeta(), p2.eeta()));
  assert (Athena_test::isEqual (p1.epT(), p2.epT()));
  assert (p1.surfaceType() == p2.surfaceType());
  assert (p1.surfaceCoordinate() == p2.surfaceCoordinate());
  const std::vector<double>& cov1 = *p1.cov();
  const std::vector<double>& cov2 = *p2.cov();
  assert (cov1.size() == cov2.size());
  for (size_t i = 0; i < cov1.size(); i++)
    assert (Athena_test::isEqual (cov1[i], cov2[i]));
}


void testit (const TrigInDetTrackFitPar& trans1)
{
  MsgStream log (0, "test");
  TrigInDetTrackFitParCnv_p1 cnv;
  TrigInDetTrackFitPar_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigInDetTrackFitPar trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  std::vector<double> cov1
  { 201.5*201.5, 202.5, 203.5, 204.5, 205.5,
    206.5*206.5, 207.5, 208.5, 209.5,
    210.5*210.5, 211.5, 212.5,
      213.5*213.5, 214.5,
      215.5*215.5 };
  TrigInDetTrackFitPar trans1
    (2.5, 3.5, 4.5, 5.5, 6.5, TrigInDetTrackFitPar::PERIGEE, 7.5,
    new std::vector<double> (cov1));

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
