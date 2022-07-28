/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file JetTagInfoTPCnv/test/SecVtxInfoCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "JetTagInfoTPCnv/SecVtxInfoCnv_p1.h"
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


void compare (const Trk::Vertex& p1,
              const Trk::Vertex& p2)
{
  assert (p1.position() == p2.position());
}


void compare (const Trk::FitQuality& p1,
              const Trk::FitQuality& p2)
{
  assert (p1.chiSquared() == p2.chiSquared());
  assert (p1.numberDoF()  == p2.numberDoF());
}


void compare (const Trk::RecVertex& p1,
              const Trk::RecVertex& p2)
{
  compare (static_cast<const Trk::Vertex&>(p1),
           static_cast<const Trk::Vertex&>(p2));
  assert (p1.covariancePosition() == p2.covariancePosition());
  compare (p1.fitQuality(), p2.fitQuality());
}


void compare (const Analysis::SecVtxInfo& p1,
              const Analysis::SecVtxInfo& p2)
{
  compare (static_cast<const Analysis::BaseTagInfo&>(p1),
           static_cast<const Analysis::BaseTagInfo&>(p2));
  assert (p1.distance() == p2.distance());
  assert (p1.rphidistance() == p2.rphidistance());
  assert (p1.probability() == p2.probability());
  assert (p1.mass() == p2.mass());
  assert (p1.energyFraction() == p2.energyFraction());
  assert (p1.mult() == p2.mult());
  assert (p1.fitType() == p2.fitType());
  compare (p1.secVertexPos(), p2.secVertexPos()); // RecVertex
  assert (p1.NumberOfG2TrackVertices() == p2.NumberOfG2TrackVertices());
}


void testit (const Analysis::SecVtxInfo& trans1)
{
  MsgStream log (0, "test");
  Analysis::SecVtxInfoCnv_p1 cnv;
  Analysis::JetTagInfoCnv_tlp3 tlcnv;
  cnv.setTopConverter (&tlcnv, TPObjRef::typeID_t());
  Analysis::SecVtxInfo_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  Analysis::SecVtxInfo trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1 ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "test1\n";
  Athena_test::Leakcheck check;

  Analysis::SecVtxInfo trans1 ("typ1");
  trans1.setWeight (1.5);
  trans1.setTagLikelihood ({2.5, 3.5, 4.5});
  trans1.makeValid();
  trans1.setNumSelTracksForFit (11);
  trans1.setDist (12.5);
  trans1.setRPhiDist (13.5);
  trans1.setMass (14.5);
  trans1.setEnergyFraction (15.5);
  trans1.setMult (16);
  trans1.setNumberOfG2TrackVertices (17);
  trans1.setFitType (Analysis::SecVtxInfo::BuildUp);

  Amg::Vector3D pos1 (10.5, 11.5, 12.5);
  AmgSymMatrix(3) cov1;
  for (int i=0; i < 3; i++) {
    for (int j=0; j<3; j++) {
      cov1(i,j) = 100*(i+1)*(j+1);
    }
  }
  Trk::RecVertex rv1 (pos1, cov1, 21.5, 22.5);
  trans1.setSecVtx (rv1, 20.5, {});

  testit (trans1);
}


int main ATLAS_NOT_THREAD_SAFE ()
{
  std::cout << "JetTagInfoTPCnv/SecVtxInfoCnv_p1_test\n";
  test1();
  return 0;
}
