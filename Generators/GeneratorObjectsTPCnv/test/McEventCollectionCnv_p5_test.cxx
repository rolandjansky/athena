/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file GeneratorObjectsTPCnv/test/McEventCollectionCnv_p5_test.cxx
 * @brief Tests for McEventCollectionCnv_p5.
 */


#undef NDEBUG
#include <cassert>
#include <iostream>
// HepMC includes
#include "AtlasHepMC/GenEvent.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

#include "GeneratorObjectsTPCnv/McEventCollectionCnv_p5.h"

void compare (const HepMC::GenEvent& p1,
              const HepMC::GenEvent& p2)
{
  assert (HepMC::signal_process_id(p1) == HepMC::signal_process_id(p2) );
  assert (p1.event_number() == p2.event_number() );
  //FIXME Need to loop over GenVertex and GenParticle objects too.
}

void compare (const McEventCollection& p1,
              const McEventCollection& p2)
{
  assert (p1.size() == p2.size());
  for (size_t i = 0; i < p1.size(); i++) {
    compare (*(p1.at(i)), *(p2.at(i)));
  }
}

void populateGenEvent(HepMC::GenEvent & ge)
{
  HepMC::FourVector myPos( 0.0, 0.0, 0.0, 0.0);
  HepMC::GenVertexPtr myVertex = HepMC::newGenVertexPtr( myPos, -1 );
  HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
  HepMC::GenParticlePtr inParticle1 = HepMC::newGenParticlePtr(fourMomentum1, 2, 10);
  myVertex->add_particle_in(inParticle1);
  HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
  HepMC::GenParticlePtr inParticle2 = HepMC::newGenParticlePtr(fourMomentum2, -2, 10);
  myVertex->add_particle_in(inParticle2);
  HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
  HepMC::GenParticlePtr inParticle3 = HepMC::newGenParticlePtr(fourMomentum3, 2, 10);
  myVertex->add_particle_out(inParticle3);
  HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
  HepMC::GenParticlePtr inParticle4 = HepMC::newGenParticlePtr(fourMomentum4, -2, 10);
  myVertex->add_particle_out(inParticle4);
  ge.add_vertex( myVertex );
  HepMC::set_signal_process_vertex(&ge, myVertex );
  ge.set_beam_particles(inParticle1,inParticle2);
}

void populateGenEvent2(HepMC::GenEvent & ge)
{
  HepMC::FourVector myPos( 0.0, 0.0, 0.0, 0.0);
  HepMC::GenVertexPtr myVertex = HepMC::newGenVertexPtr( myPos, -1 );
  HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
  HepMC::GenParticlePtr inParticle1 = HepMC::newGenParticlePtr(fourMomentum1, 2, 10);
  myVertex->add_particle_in(inParticle1);
  HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
  HepMC::GenParticlePtr inParticle2 = HepMC::newGenParticlePtr(fourMomentum2, -2, 10);
  myVertex->add_particle_in(inParticle2);
  HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
  HepMC::GenParticlePtr inParticle3 = HepMC::newGenParticlePtr(fourMomentum3, 2, 10);
  myVertex->add_particle_out(inParticle3);
  HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
  HepMC::GenParticlePtr inParticle4 = HepMC::newGenParticlePtr(fourMomentum4, -2, 10);
  myVertex->add_particle_out(inParticle4);
  ge.add_vertex( myVertex );
  HepMC::set_signal_process_vertex(&ge, myVertex );
  ge.set_beam_particles(inParticle1,inParticle2);
}

void testit (const McEventCollection& trans1)
{
  MsgStream log (0, "test");
  McEventCollectionCnv_p5 cnv;
  McEventCollection_p5 pers;
  cnv.transToPers (&trans1, &pers, log);
  McEventCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}

void test1()
{
  std::cout << "test1\n";

  McEventCollection trans1;
  // Add a dummy GenEvent
  const int process_id1(20);
  const int event_number1(17);
  trans1.push_back(HepMC::newGenEvent(process_id1, event_number1));
  HepMC::GenEvent& ge1 = *(trans1.at(0));
  populateGenEvent(ge1);
  // Add a second dummy GenEvent
  const int process_id2(20);
  const int event_number2(25);
  trans1.push_back(HepMC::newGenEvent(process_id2, event_number2));
  HepMC::GenEvent& ge2 = *(trans1.at(1));
  populateGenEvent2(ge2);

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
