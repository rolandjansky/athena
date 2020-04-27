/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file GeneratorObjectsTPCnv/test/HepMcParticleLinkCnv_p2_test.cxx
 * @brief Tests for HepMcParticleLinkCnv_p2.
 */


#undef NDEBUG
#include <cassert>
#include <iostream>
#include <vector>
// HepMC includes
#include "AtlasHepMC/GenEvent.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p2.h"
#include "StoreGate/WriteHandle.h"
#include "GeneratorObjects/McEventCollection.h"

#include "TestTools/initGaudi.h"

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

void populateGenEvent(HepMC::GenEvent & ge, int pdgid1, int pdgid2, std::vector<HepMC::GenParticle*>& genPartList)
{
  CLHEP::HepLorentzVector myPos( 0.0, 0.0, 0.0, 0.0);
  HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
  HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
  HepMC::GenParticle* inParticle1 = new HepMC::GenParticle(fourMomentum1, pdgid1, 2);
  myVertex->add_particle_in(inParticle1);
  HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
  HepMC::GenParticle* inParticle2 = new HepMC::GenParticle(fourMomentum2, pdgid2, 2);
  myVertex->add_particle_in(inParticle2);
  HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
  HepMC::GenParticle* inParticle3 = new HepMC::GenParticle(fourMomentum3, pdgid1, 1);
  myVertex->add_particle_out(inParticle3);
  genPartList.push_back(inParticle3);
  HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
  HepMC::GenParticle* inParticle4 = new HepMC::GenParticle(fourMomentum4, pdgid2, 1);
  myVertex->add_particle_out(inParticle4);
  genPartList.push_back(inParticle4);
  ge.add_vertex( myVertex );
  ge.set_signal_process_vertex( myVertex );
  ge.set_beam_particles(inParticle1,inParticle2);
}

void createMcEventCollectionInStoreGate(std::vector<HepMC::GenParticle*>& genPartList)
{
  // create dummy input McEventCollection with a name that
  // HepMcParticleLink knows about
  SG::WriteHandle<McEventCollection> inputTestDataHandle{"TruthEvent"};
  inputTestDataHandle = std::make_unique<McEventCollection>();
  // Add a dummy GenEvent
  const int process_id1(20);
  const int event_number1(17);
  const int event_number2(18);
  const int event_number3(64);
  inputTestDataHandle->push_back(new HepMC::GenEvent(process_id1, event_number1));
  HepMC::GenEvent& ge1 = *(inputTestDataHandle->at(0));
  populateGenEvent(ge1,-11,11,genPartList);
  populateGenEvent(ge1,-13,13,genPartList);
  inputTestDataHandle->push_back(new HepMC::GenEvent(process_id1, event_number2));
  HepMC::GenEvent& ge2 = *(inputTestDataHandle->at(1));
  populateGenEvent(ge2,-11,11,genPartList);
  populateGenEvent(ge2,-13,13,genPartList);
  inputTestDataHandle->push_back(new HepMC::GenEvent(process_id1, event_number3));
  HepMC::GenEvent& ge3 = *(inputTestDataHandle->at(2));
  populateGenEvent(ge3,-11,11,genPartList);
  populateGenEvent(ge3,22,22,genPartList);
  return;
}

void testit (const HepMcParticleLink& trans1)
{
  MsgStream log (0, "test");
  HepMcParticleLinkCnv_p2 cnv;
  HepMcParticleLink_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  HepMcParticleLink trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}

void test1()
{
  std::cout << "test1\n";
  std::vector<HepMC::GenParticle*> genPartList;
  createMcEventCollectionInStoreGate(genPartList);
  assert ( genPartList.size() == 12 );
  const HepMC::GenParticle * particle1 = genPartList.at(0);
  HepMcParticleLink trans1(particle1->barcode(),particle1->parent_event()->event_number());
  testit (trans1);
  HepMcParticleLink trans2(particle1->barcode(),0,EBC_MAINEVCOLL,HepMcParticleLink::IS_POSITION);
  testit (trans2);
  const HepMC::GenParticle * particle2 = genPartList.at(7);
  HepMcParticleLink trans3(particle2->barcode(),particle2->parent_event()->event_number());
  testit (trans3);
  const HepMC::GenParticle * particle3 = genPartList.at(8);
  HepMcParticleLink trans4(particle3->barcode(),particle3->parent_event()->event_number());
  testit (trans4);
}


int main()
{
  ISvcLocator* pSvcLoc = nullptr;
  if (!Athena_test::initGaudi(pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  test1();
  return 0;
}
