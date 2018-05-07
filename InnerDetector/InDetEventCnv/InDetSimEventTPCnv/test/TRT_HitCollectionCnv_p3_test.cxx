/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file InDetSimEventTPCnv/test/TRT_HitCollectionCnv_p3_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for TRT_HitCollectionCnv_p3.
 */


#undef NDEBUG
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p3.h"
#include "TestTools/FLOATassert.h"
#include "TestTools/leakcheck.h"
#include <cassert>
#include <iostream>
#include <cmath>

#include "GeneratorObjectsTPCnv/initMcEventCollection.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"

using Athena_test::isEqual;
using std::atan2;


void compare (const HepMcParticleLink& p1,
              const HepMcParticleLink& p2)
{
  assert ( p1.isValid() == p2.isValid() );
  assert ( p1.barcode() == p2.barcode() );
  assert ( p1.eventIndex() == p2.eventIndex() );
  assert ( p1.getEventCollectionAsChar() == p2.getEventCollectionAsChar() );
  assert ( p1.cptr() == p2.cptr() );
  assert ( p1 == p2 );
}

void compare (const TRTUncompressedHit& p1,
              const TRTUncompressedHit& p2)
{
  assert (p1.GetHitID() == p2.GetHitID());
  compare(p1.particleLink(), p2.particleLink());
  assert (p1.particleLink() == p2.particleLink());
  assert (p1.GetParticleEncoding() == p2.GetParticleEncoding());
  assert (isEqual (p1.GetKineticEnergy(), p2.GetKineticEnergy(), 5e-4));
  if (p1.GetParticleEncoding() == 22)
    assert (p1.GetEnergyDeposit() == p2.GetEnergyDeposit());
  else
    assert (0 == p2.GetEnergyDeposit());
  if (p1.GetPreStepX() > 2) {
    const double phi1 = atan2 (p1.GetPreStepY(), p1.GetPreStepX());
    assert (isEqual (2*cos(phi1), p2.GetPreStepX(), 1e-2));
    assert (isEqual (2*sin(phi1), p2.GetPreStepY(), 2e-2));

    const double phi2 = atan2 (p1.GetPostStepY(), p1.GetPostStepX());
    assert (isEqual (2*cos(phi2), p2.GetPostStepX(), 2e-2));
    assert (isEqual (2*sin(phi2), p2.GetPostStepY(), 1e-2));
  }
  else {
    assert (isEqual (p1.GetPreStepX(), p2.GetPreStepX(), 1e-2));
    assert (isEqual (p1.GetPreStepY(), p2.GetPreStepY(), 1e-2));
    assert (isEqual (p1.GetPostStepX(), p2.GetPostStepX(), 2e-2));
    assert (isEqual (p1.GetPostStepY(), p2.GetPostStepY(), 2e-2));
  }
  assert (isEqual (p1.GetPreStepZ(), p2.GetPreStepZ(), 0.1));
  assert (isEqual (p1.GetPostStepZ(), p2.GetPostStepZ(), 0.1));
  assert (p1.GetGlobalTime() == p2.GetGlobalTime());
}


void compare (const TRTUncompressedHitCollection& p1,
              const TRTUncompressedHitCollection& p2)
{
  //assert (p1.Name() == p2.Name());
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++)
    compare (p1[i], p2[i]);
}


void testit (const TRTUncompressedHitCollection& trans1)
{
  MsgStream log (0, "test");
  TRT_HitCollectionCnv_p3 cnv;
  TRT_HitCollection_p3 pers;
  cnv.transToPers (&trans1, &pers, log);
  TRTUncompressedHitCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1(std::vector<HepMC::GenParticle*>& genPartVector)
{
  std::cout << "test1\n";
  const HepMC::GenParticle *particle = genPartVector.at(0);
  // Create HepMcParticleLink outside of leak check.
  HepMcParticleLink dummyHMPL(particle->barcode(),particle->parent_event()->event_number());
  assert(dummyHMPL.cptr()==particle);
  // Create DVL info outside of leak check.
  TRTUncompressedHitCollection dum ("coll");
  Athena_test::Leakcheck check;

  TRTUncompressedHitCollection trans1 ("coll");
  for (int i=0; i < 10; i++) {
    int o = i*100;
    const HepMC::GenParticle* pGenParticle = genPartVector.at(0);
    HepMcParticleLink trkLink(pGenParticle->barcode(),pGenParticle->parent_event()->event_number());
    trans1.Emplace (101+o, trkLink, 20+o,
                    104.5+o, 105.5+o,
                    (106.5+o)/1000, (107.5+o)/1000, 108.5+o,
                    (109.5+o)/1000, (110.5+o)/1000, 111.5+o,
                    112.5+o);
  }
  // Special case for photons
  const HepMC::GenParticle* pGenParticle = genPartVector.at(10);
  HepMcParticleLink trkLink(pGenParticle->barcode(),pGenParticle->parent_event()->event_number());
  trans1.Emplace (131, trkLink, 22,
                  134.5, 135.5,
                  10, 3, 138.5,
                  3, 10, 148.5,
                  142.5);

  testit (trans1);
}


int main()
{
  ISvcLocator* pSvcLoc = nullptr;
  std::vector<HepMC::GenParticle*> genPartVector;
  if (!Athena_test::initMcEventCollection(pSvcLoc, genPartVector)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  test1(genPartVector);
  return 0;
}
