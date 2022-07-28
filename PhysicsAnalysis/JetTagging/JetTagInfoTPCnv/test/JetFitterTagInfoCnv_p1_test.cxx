/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/JetFitterTagInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/JetFitterTagInfoCnv_p1.h"
#include "JetTagInfoTPCnv/JetTagInfoCnv_tlp3.h"
#include "CxxUtils/checker_macros.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const Analysis::BaseTagInfo& p1,
              const Analysis::BaseTagInfo& p2)
{
  assert (p1.isValid() == p2.isValid());
  assert (p1.tagLikelihood() == p2.tagLikelihood());
  assert (p1.weight() == p2.weight());
  assert (p1.infoType() == p2.infoType());
}


void compare (const Analysis::JetFitterTagInfo& p1,
              const Analysis::JetFitterTagInfo& p2)
{
  compare (static_cast<const Analysis::BaseTagInfo&>(p1),
           static_cast<const Analysis::BaseTagInfo&>(p2));
  assert (p1.nVTX() == p2.nVTX());
  assert (p1.nSingleTracks() == p2.nSingleTracks());
  assert (p1.nTracksAtVtx() == p2.nTracksAtVtx());
  assert (p1.mass() == p2.mass());
  assert (p1.energyFraction() == p2.energyFraction());
  assert (p1.significance3d() == p2.significance3d());
  assert (p1.deltaeta() == p2.deltaeta());
  assert (p1.deltaphi() == p2.deltaphi());
}


void testit (const Analysis::JetFitterTagInfo& trans1)
{
  MsgStream log (0, "test");
  Analysis::JetFitterTagInfoCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::JetFitterTagInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::JetFitterTagInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Analysis::JetFitterTagInfo trans1 ("typ");
  trans1.setWeight (1.5);
  trans1.setTagLikelihood ({2.5, 3.5, 4.5});
  trans1.makeValid();
  trans1.setnVTX (10);
  trans1.setnSingleTracks (11);
  trans1.setnTracksAtVtx (11);
  trans1.setMass (5.5);
  trans1.setEnergyFraction (6.5);
  trans1.setSignificance3d (7.5);
  trans1.setDeltaeta (8.5);
  trans1.setDeltaphi (9.5);
  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "JetTagInfoTPCnv/JetFitterTagInfoCnv_p1_test\n";
  test1();
  return 0;
}
