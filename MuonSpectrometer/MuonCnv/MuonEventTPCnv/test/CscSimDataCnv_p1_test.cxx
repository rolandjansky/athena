/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/CscSimDataCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonDigitContainer/CscSimDataCnv_p1.h"
#include "TestTools/leakcheck.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>

#include "GeneratorObjectsTPCnv/initMcEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"


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


void compare (const CscMcData& p1,
              const CscMcData& p2)
{
  assert (p1.energy() == p2.energy());
  assert (p1.ypos() == p2.ypos());
  assert (p1.zpos() == p2.zpos());
  assert (p1.charge() == p2.charge());
}


void compare (const CscSimData& p1,
              const CscSimData& p2)
{
  assert (p1.word() == p2.word());
  const std::vector< CscSimData::Deposit >& dep1 = p1.getdeposits();
  const std::vector< CscSimData::Deposit >& dep2 = p2.getdeposits();
  assert (dep1.size() == dep2.size());
  for (size_t i = 0; i < dep1.size(); i++) {
    compare (dep1[i].first, dep2[i].first);
    assert (dep1[i].first == dep2[i].first);
    compare (dep1[i].second, dep2[i].second);
  }
}


void testit (const CscSimData& trans1)
{
  MsgStream log (0, "test");
  CscSimDataCnv_p1 cnv;
  Muon::CscSimData_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  CscSimData trans2;
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
  Athena_test::Leakcheck check;

  std::vector<CscSimData::Deposit> deps;
  HepMcParticleLink trkLink1(genPartVector.at(0)->barcode(),genPartVector.at(0)->parent_event()->event_number());
  deps.emplace_back (trkLink1, CscMcData ( 2.5,  3.5,  4.5));
  HepMcParticleLink trkLink2(genPartVector.at(1)->barcode(),genPartVector.at(1)->parent_event()->event_number());
  deps.emplace_back (trkLink2, CscMcData (12.5, 13.5, 14.5));
  HepMcParticleLink trkLink3(genPartVector.at(2)->barcode(),genPartVector.at(2)->parent_event()->event_number());
  deps.emplace_back (trkLink3, CscMcData (22.5, 23.5, 24.5));
  deps[0].second.setCharge ( 5.5);
  deps[1].second.setCharge (15.5);
  deps[2].second.setCharge (25.5);
  CscSimData trans1 (deps, 4321);
  testit (trans1);
}


int main()
{
  ISvcLocator* pSvcLoc = nullptr;
  std::vector<HepMC::GenParticle*> genPartVector;
  if (!Athena_test::initMcEventCollection(pSvcLoc,genPartVector)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  test1(genPartVector);
  return 0;
}
