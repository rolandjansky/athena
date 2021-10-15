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
#include "HepMC/GenEvent.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "GeneratorObjectsTPCnv/McEventCollectionCnv_p5.h"

void compareGenParticle(const HepMC::GenParticle& p1,
                        const HepMC::GenParticle& p2)
{
  assert (p1.barcode() == p2.barcode());
  assert (p1.status() == p2.status());
  assert (p1.pdg_id() == p2.pdg_id());
  assert ((p1.momentum().px()) == (p2.momentum().px()));
  assert ((p1.momentum().py()) == (p2.momentum().py()));
  assert ((p1.momentum().pz()) == (p2.momentum().pz()));
  assert (float(p1.momentum().m()) == float(p2.momentum().m())); // only persistified with float precision

  return;
}


void compareGenVertex(const HepMC::GenVertex& v1,
                      const HepMC::GenVertex& v2)
{
  assert (v1.barcode() == v2.barcode());
  assert (float(v1.position().x()) == float(v2.position().x())); // only persistified with float precision
  assert (float(v1.position().y()) == float(v2.position().y())); // only persistified with float precision
  assert (float(v1.position().z()) == float(v2.position().z())); // only persistified with float precision
  assert (float(v1.position().t()) == float(v2.position().t())); // only persistified with float precision
  assert (v1.particles_in_size() == v2.particles_in_size());
  assert (v1.particles_out_size() == v2.particles_out_size());

  HepMC::GenVertex::particles_in_const_iterator originalPartInIter(v1.particles_in_const_begin());
  const HepMC::GenVertex::particles_in_const_iterator endOfOriginalListOfParticlesIn(v1.particles_in_const_end());
  HepMC::GenVertex::particles_in_const_iterator resetPartInIter(v2.particles_in_const_begin());
  const HepMC::GenVertex::particles_in_const_iterator endOfResetListOfParticlesIn(v2.particles_in_const_end());
  while( originalPartInIter!=endOfOriginalListOfParticlesIn &&
         resetPartInIter!=endOfResetListOfParticlesIn ) {
    compareGenParticle(**originalPartInIter,**resetPartInIter);
    ++resetPartInIter;
    ++originalPartInIter;
  }

  HepMC::GenVertex::particles_out_const_iterator originalPartOutIter(v1.particles_out_const_begin());
  const HepMC::GenVertex::particles_out_const_iterator endOfOriginalListOfParticlesOut(v1.particles_out_const_end());
  HepMC::GenVertex::particles_out_const_iterator resetPartOutIter(v2.particles_out_const_begin());
  const HepMC::GenVertex::particles_out_const_iterator endOfResetListOfParticlesOut(v2.particles_out_const_end());
  while( originalPartOutIter!=endOfOriginalListOfParticlesOut &&
         resetPartOutIter!=endOfResetListOfParticlesOut ) {
    compareGenParticle(**originalPartOutIter,**resetPartOutIter);
    ++resetPartOutIter;
    ++originalPartOutIter;
  }

  return;
}


void compare (const HepMC::GenEvent& p1,
              const HepMC::GenEvent& p2)
{
  assert (p1.signal_process_id() == p2.signal_process_id());
  assert (p1.event_number() == p2.event_number());
  assert (p1.valid_beam_particles() == p2.valid_beam_particles());
  if ( p1.valid_beam_particles() && p2.valid_beam_particles() ) {
    std::pair<HepMC::GenParticle*,HepMC::GenParticle*> originalBP = p1.beam_particles();
    std::pair<HepMC::GenParticle*,HepMC::GenParticle*> resetBP = p2.beam_particles();
    compareGenParticle(*(originalBP.first), *(resetBP.first));
    compareGenParticle(*(originalBP.second), *(resetBP.second));
  }

  assert (p1.particles_size() == p2.particles_size());
  assert (p1.vertices_size() == p2.vertices_size());

  HepMC::GenEvent::particle_const_iterator origParticleIter(p1.particles_begin());
  const HepMC::GenEvent::particle_const_iterator endOfOriginalListOfParticles(p1.particles_end());
  HepMC::GenEvent::particle_const_iterator resetParticleIter(p2.particles_begin());
  const HepMC::GenEvent::particle_const_iterator endOfResetListOfParticles(p2.particles_end());
  while( origParticleIter!=endOfOriginalListOfParticles &&
         resetParticleIter!=endOfResetListOfParticles ) {
    compareGenParticle(**origParticleIter,**resetParticleIter);
    ++origParticleIter;
    ++resetParticleIter;
  }

  HepMC::GenEvent::vertex_const_iterator origVertexIter(p1.vertices_begin());
  const HepMC::GenEvent::vertex_const_iterator endOfOriginalListOfVertices(p1.vertices_end());
  HepMC::GenEvent::vertex_const_iterator resetVertexIter(p2.vertices_begin());
  const HepMC::GenEvent::vertex_const_iterator endOfResetListOfVertices(p2.vertices_end());
  while( origVertexIter!=endOfOriginalListOfVertices &&
         resetVertexIter!=endOfResetListOfVertices ) {
    compareGenVertex(**origVertexIter,**resetVertexIter);
    ++origVertexIter;
    ++resetVertexIter;
  }
  return;
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
  CLHEP::HepLorentzVector myPos( 0.0345682751, 0.00872347682, 0.23671987, 0.0);
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
}

void populateGenEvent2(HepMC::GenEvent & ge)
{
  CLHEP::HepLorentzVector myPos( 0.0054625871, 0.08027374862, 0.32769178, 0.0);
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
}

void testit (const McEventCollection& trans1)
{
  MsgStream log (0, "test");
  McEventCollectionCnv_p5 cnv;
  McEventCollection_p5 pers;
  cnv.transToPers (&trans1, &pers, log);
  McEventCollection trans_tmp;
  cnv.persToTrans (&pers, &trans_tmp, log);
  McEventCollection_p5 pers2;
  cnv.transToPers (&trans_tmp, &pers2, log);
  McEventCollection trans2;
  cnv.persToTrans (&pers2, &trans2, log);

  compare (trans1, trans2);
}

void test1()
{
  std::cout << "test1\n";

  McEventCollection trans1;
  // Add a dummy GenEvent
  const int process_id1(20);
  const int event_number1(17);
  trans1.push_back(new HepMC::GenEvent(process_id1, event_number1));
  HepMC::GenEvent& ge1 = *(trans1.at(0));
  populateGenEvent(ge1);
  // Add a second dummy GenEvent
  const int process_id2(20);
  const int event_number2(25);
  trans1.push_back(new HepMC::GenEvent(process_id2, event_number2));
  HepMC::GenEvent& ge2 = *(trans1.at(1));
  populateGenEvent2(ge2);

  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
