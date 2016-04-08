/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigParticleTPCnv/test/TrigL2BjetCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TrigL2BjetCnv_p2.
 */


#undef NDEBUG
#include "TrigParticleTPCnv/TrigL2BjetCnv_p2.h"
#include "TrigParticleTPCnv/TrigL2BjetContainerCnv_tlp2.h"
#include "TestTools/FLOATassert.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const P4PtEtaPhiM& p1,
              const P4PtEtaPhiM& p2)
{
  assert (Athena_test::isEqual (p1.pt(), p2.pt()));
  assert (Athena_test::isEqual (p1.eta(), p2.eta()));
  assert (Athena_test::isEqual (p1.phi(), p2.phi()));
  assert (Athena_test::isEqual (p1.m(), p2.m()));
}


void compare (const TrigL2Bjet& p1,
              const TrigL2Bjet& p2)
{
  compare (static_cast<const P4PtEtaPhiM&>(p1),
           static_cast<const P4PtEtaPhiM&>(p2));

  assert (p1.roiId() == p2.roiId());
  assert (p1.isValid() == p2.isValid());
  assert (p1.prmVtx() == p2.prmVtx());
  assert (p1.xComb() == p2.xComb());
  assert (p1.xIP1D() == p2.xIP1D());
  assert (p1.xIP2D() == p2.xIP2D());
  assert (p1.xIP3D() == p2.xIP3D());
  assert (p1.xCHI2() == p2.xCHI2());
  assert (p1.xSV() == p2.xSV());
  assert (p1.xMVtx() == p2.xMVtx());
  assert (p1.xEVtx() == p2.xEVtx());
  assert (p1.xNVtx() == p2.xNVtx());
}


void testit (const TrigL2Bjet& trans1)
{
  MsgStream log (0, "test");
  TrigL2BjetCnv_p2 cnv;
  TrigL2BjetContainerCnv_tlp2 tlcnv;
  cnv.setRuntimeTopConverter (&tlcnv);
  TrigL2Bjet_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  TrigL2Bjet trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  TrigL2Bjet trans1 (123, 2.5, 1.5,
                     nullptr,
                     nullptr,
                     nullptr,
                     3.5,  // prmvtx
                     4.5,  // ptJet
                     5.5,  // xcomb
                     6.5,  // xIP1d
                     7.5,  // xIP2d
                     8.5,  // xIP3d
                     9.5,  // xChi2
                     10.5, // xSv
                     11.5, // xmvtx
                     12.5, // xevtx
                     13.5  // nvtx
                     );
  trans1.validate(true);
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
