/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file GeneratorObjectsTPCnv/test/HepMcParticleLinkCnv_p1_test.cxx
 * @brief Tests for HepMcParticleLinkCnv_p1.
 */


#undef NDEBUG
#include <cassert>
#include <iostream>
// HepMC includes
#include "HepMC/GenEvent.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"
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

HepMC::GenParticle* populateGenEvent(HepMC::GenEvent & ge)
{
  CLHEP::HepLorentzVector myPos( 0.0, 0.0, 0.0, 0.0);
  HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
  HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
  HepMC::GenParticle* inParticle1 = new HepMC::GenParticle(fourMomentum1, 2, 10);
  myVertex->add_particle_in(inParticle1);
  HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
  HepMC::GenParticle* inParticle2 = new HepMC::GenParticle(fourMomentum2, -2, 10);
  myVertex->add_particle_in(inParticle2);
  HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
  HepMC::GenParticle* inParticle3 = new HepMC::GenParticle(fourMomentum3, 2, 10);
  myVertex->add_particle_out(inParticle3);
  HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
  HepMC::GenParticle* inParticle4 = new HepMC::GenParticle(fourMomentum4, -2, 10);
  myVertex->add_particle_out(inParticle4);
  ge.add_vertex( myVertex );
  ge.set_signal_process_vertex( myVertex );
  ge.set_beam_particles(inParticle1,inParticle2);
  return inParticle3;
}

HepMC::GenParticle* createMcEventCollectionInStoreGate()
{
  // create dummy input McEventCollection with a name that
  // HepMcParticleLink knows about
  SG::WriteHandle<McEventCollection> inputTestDataHandle{"TruthEvent"};
  inputTestDataHandle = std::make_unique<McEventCollection>();
  // Add a dummy GenEvent
  const int process_id1(20);
  const int event_number1(17);
  inputTestDataHandle->push_back(new HepMC::GenEvent(process_id1, event_number1));
  HepMC::GenEvent& ge1 = *(inputTestDataHandle->at(0));
  return populateGenEvent(ge1);
}

void testit (const HepMcParticleLink& trans1)
{
  MsgStream log (0, "test");
  HepMcParticleLinkCnv_p1 cnv;
  HepMcParticleLink_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  HepMcParticleLink trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}

void test1()
{
  std::cout << "test1\n";
  const HepMC::GenParticle * particle = createMcEventCollectionInStoreGate();

  HepMcParticleLink trans1(particle->barcode(),particle->parent_event()->event_number());

  testit (trans1);
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
