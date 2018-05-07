/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonEventTPCnv/test/MuonSimDataCollectionCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2015
 * @brief Regression tests.
 */

#undef NDEBUG
#include "MuonEventTPCnv/MuonDigitContainer/MuonSimDataCollectionCnv_p2.h"
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


void compare (const MuonMCData& p1,
              const MuonMCData& p2)
{
  assert (p1.firstEntry() == p2.firstEntry());
  assert (p1.secondEntry() == p2.secondEntry());
}


void compare (const MuonSimData& p1,
              const MuonSimData& p2)
{
  assert (p1.word() == p2.word());
  assert (p1.globalPosition() == p2.globalPosition());
  const std::vector< MuonSimData::Deposit >& dep1 = p1.getdeposits();
  const std::vector< MuonSimData::Deposit >& dep2 = p2.getdeposits();
  assert (dep1.size() == dep2.size());
  for (size_t i = 0; i < dep1.size(); i++) {
    compare (dep1[i].first, dep2[i].first);
    assert (dep1[i].first == dep2[i].first);
    compare (dep1[i].second, dep2[i].second);
  }
}


void compare (const MuonSimDataCollection& p1,
              const MuonSimDataCollection& p2)
{
  assert (p1.size() == p2.size());
  MuonSimDataCollection::const_iterator it1 = p1.begin();
  MuonSimDataCollection::const_iterator it2 = p2.begin();
  for (; it1 != p1.end(); ++it1, ++it2) {
    assert (it1->first == it2->first);
    compare (it1->second, it2->second);
  }
}


void testit (const MuonSimDataCollection& trans1)
{
  MsgStream log (0, "test");
  MuonSimDataCollectionCnv_p2 cnv;
  Muon::MuonSimDataCollection_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  MuonSimDataCollection trans2;
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

  MuonSimDataCollection trans1;
  for (int i=0; i < 3; i++) {
    std::vector<MuonSimData::Deposit> deps;
    HepMcParticleLink trkLink1(genPartVector.at(0+(3*i))->barcode(),genPartVector.at(0+(3*i))->parent_event()->event_number());
    deps.emplace_back (trkLink1, MuonMCData ( 2.5+i,  3.5+i));
    HepMcParticleLink trkLink2(genPartVector.at(1+(3*i))->barcode(),genPartVector.at(1+(3*i))->parent_event()->event_number());
    deps.emplace_back (trkLink2, MuonMCData (12.5+i, 13.5+i));
    HepMcParticleLink trkLink3(genPartVector.at(2+(3*i))->barcode(),genPartVector.at(2+(3*i))->parent_event()->event_number());
    deps.emplace_back (trkLink3, MuonMCData (22.5+i, 23.5+i));
    trans1[Identifier(1234+i)] = MuonSimData (deps, 4321+i);
    trans1[Identifier(1234+i)].setPosition (Amg::Vector3D(4.5+i, 5.5+i, 6.5+i));
  }
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
