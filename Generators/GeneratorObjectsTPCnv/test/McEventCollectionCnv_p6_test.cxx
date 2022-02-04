/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file GeneratorObjectsTPCnv/test/McEventCollectionCnv_p6_test.cxx
 * @brief Tests for McEventCollectionCnv_p6.
 */


#undef NDEBUG
#include <cassert>
#include <iostream>
// HepMC includes
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"
#include "TestTools/initGaudi.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

#include "GeneratorObjectsTPCnv/McEventCollectionCnv_p6.h"

void compareGenParticle(HepMC::ConstGenParticlePtr p1,
                        HepMC::ConstGenParticlePtr p2)
{
  assert (HepMC::barcode(p1) == HepMC::barcode(p2));
  assert (p1->status() == p2->status());
  assert (p1->pdg_id() == p2->pdg_id());
  assert ((p1->momentum().px()) == (p2->momentum().px()));
  assert ((p1->momentum().py()) == (p2->momentum().py()));
  assert ((p1->momentum().pz()) == (p2->momentum().pz()));
  assert (float(p1->momentum().m()) == float(p2->momentum().m())); // only persistified with float precision

  return;
}


void compareGenVertex(HepMC::ConstGenVertexPtr v1,
                      HepMC::ConstGenVertexPtr v2)
{
  assert (HepMC::barcode(v1) == HepMC::barcode(v2));
  assert (float(v1->position().x()) == float(v2->position().x())); // only persistified with float precision
  assert (float(v1->position().y()) == float(v2->position().y())); // only persistified with float precision
  assert (float(v1->position().z()) == float(v2->position().z())); // only persistified with float precision
  assert (float(v1->position().t()) == float(v2->position().t())); // only persistified with float precision
  assert (HepMC::particles_in_size(v1) == HepMC::particles_in_size(v2));
  assert (HepMC::particles_out_size(v1) == HepMC::particles_out_size(v2));

#ifdef HEPMC3
  std::vector<HepMC::ConstGenParticlePtr>::const_iterator originalPartInIter(v1->particles_in().begin());
  const std::vector<HepMC::ConstGenParticlePtr>::const_iterator endOfOriginalListOfParticlesIn(v1->particles_in().end());
  std::vector<HepMC::ConstGenParticlePtr>::const_iterator resetPartInIter(v2->particles_in().begin());
  const std::vector<HepMC::ConstGenParticlePtr>::const_iterator endOfResetListOfParticlesIn(v2->particles_in().end());
#else
  HepMC::GenVertex::particles_in_const_iterator originalPartInIter(v1->particles_in_const_begin());
  const HepMC::GenVertex::particles_in_const_iterator endOfOriginalListOfParticlesIn(v1->particles_in_const_end());
  HepMC::GenVertex::particles_in_const_iterator resetPartInIter(v2->particles_in_const_begin());
  const HepMC::GenVertex::particles_in_const_iterator endOfResetListOfParticlesIn(v2->particles_in_const_end());
#endif
  while( originalPartInIter!=endOfOriginalListOfParticlesIn &&
         resetPartInIter!=endOfResetListOfParticlesIn ) {
    compareGenParticle(*originalPartInIter,*resetPartInIter);
    ++resetPartInIter;
    ++originalPartInIter;
  }

#ifdef HEPMC3
std::vector<HepMC::ConstGenParticlePtr>::const_iterator originalPartOutIter(v1->particles_out().begin());
const std::vector<HepMC::ConstGenParticlePtr>::const_iterator endOfOriginalListOfParticlesOut(v1->particles_out().end());
std::vector<HepMC::ConstGenParticlePtr>::const_iterator resetPartOutIter(v2->particles_out().begin());
const std::vector<HepMC::ConstGenParticlePtr>::const_iterator endOfResetListOfParticlesOut(v2->particles_out().end());
#else
HepMC::GenVertex::particles_out_const_iterator originalPartOutIter(v1->particles_out_const_begin());
const HepMC::GenVertex::particles_out_const_iterator endOfOriginalListOfParticlesOut(v1->particles_out_const_end());
HepMC::GenVertex::particles_out_const_iterator resetPartOutIter(v2->particles_out_const_begin());
const HepMC::GenVertex::particles_out_const_iterator endOfResetListOfParticlesOut(v2->particles_out_const_end());
#endif
  while( originalPartOutIter!=endOfOriginalListOfParticlesOut &&
         resetPartOutIter!=endOfResetListOfParticlesOut ) {
    compareGenParticle(*originalPartOutIter,*resetPartOutIter);
    ++resetPartOutIter;
    ++originalPartOutIter;
  }

  return;
}


void compare (const HepMC::GenEvent& e1,
              const HepMC::GenEvent& e2)
{
  assert (HepMC::signal_process_id(e1) == HepMC::signal_process_id(e2) );
  assert (e1.event_number() == e2.event_number() );

  assert (HepMC::valid_beam_particles(&e1) == HepMC::valid_beam_particles(&e2));
  if ( HepMC::valid_beam_particles(&e1) && HepMC::valid_beam_particles(&e2) ) {
#if HEPMC3
    const std::vector<HepMC::ConstGenParticlePtr> & originalBPs = e1.beams();
    const std::vector<HepMC::ConstGenParticlePtr> & resetBPs = e2.beams();
    compareGenParticle(originalBPs.at(0), resetBPs.at(0));
    compareGenParticle(originalBPs.at(1), resetBPs.at(1));
#else
    std::pair<HepMC::GenParticle*,HepMC::GenParticle*> originalBP = e1.beam_particles();
    std::pair<HepMC::GenParticle*,HepMC::GenParticle*> resetBP = e2.beam_particles();
    compareGenParticle(originalBP.first, resetBP.first);
    compareGenParticle(originalBP.second, resetBP.second);
#endif
  }

#if HEPMC3
  assert (e1.particles().size() == e2.particles().size());
  assert (e1.vertices().size() == e2.vertices().size());

  std::vector<HepMC3::ConstGenParticlePtr>::const_iterator origParticleIter(begin(e1));
  const std::vector<HepMC3::ConstGenParticlePtr>::const_iterator endOfOriginalListOfParticles(end(e1));
  std::vector<HepMC3::ConstGenParticlePtr>::const_iterator resetParticleIter(begin(e2));
  const std::vector<HepMC3::ConstGenParticlePtr>::const_iterator endOfResetListOfParticles(end(e2));

  while( origParticleIter!=endOfOriginalListOfParticles &&
         resetParticleIter!=endOfResetListOfParticles ) {
    compareGenParticle(*origParticleIter,*resetParticleIter);
    ++origParticleIter;
    ++resetParticleIter;
  }

  std::vector<HepMC::ConstGenVertexPtr>::const_iterator origVertexIter(e1.vertices().begin());
  const std::vector<HepMC::ConstGenVertexPtr>::const_iterator endOfOriginalListOfVertices(e1.vertices().end());
  std::vector<HepMC::ConstGenVertexPtr>::const_iterator resetVertexIter(e2.vertices().begin());
  const std::vector<HepMC::ConstGenVertexPtr>::const_iterator endOfResetListOfVertices(e2.vertices().end());
  while( origVertexIter!=endOfOriginalListOfVertices &&
         resetVertexIter!=endOfResetListOfVertices ) {
    compareGenVertex(*origVertexIter,*resetVertexIter);
    ++origVertexIter;
    ++resetVertexIter;
  }
#else
  assert (e1.particles_size() == e2.particles_size());
  assert (e1.vertices_size() == e2.vertices_size());

  HepMC::GenEvent::particle_const_iterator origParticleIter(begin(e1));
  const HepMC::GenEvent::particle_const_iterator endOfOriginalListOfParticles(end(e1));
  HepMC::GenEvent::particle_const_iterator resetParticleIter(begin(e2));
  const HepMC::GenEvent::particle_const_iterator endOfResetListOfParticles(end(e2));

  while( origParticleIter!=endOfOriginalListOfParticles &&
         resetParticleIter!=endOfResetListOfParticles ) {
    compareGenParticle(*origParticleIter,*resetParticleIter);
    ++origParticleIter;
    ++resetParticleIter;
  }

  HepMC::GenEvent::vertex_const_iterator origVertexIter(e1.vertices_begin());
  const HepMC::GenEvent::vertex_const_iterator endOfOriginalListOfVertices(e1.vertices_end());
  HepMC::GenEvent::vertex_const_iterator resetVertexIter(e2.vertices_begin());
  const HepMC::GenEvent::vertex_const_iterator endOfResetListOfVertices(e2.vertices_end());
  while( origVertexIter!=endOfOriginalListOfVertices &&
         resetVertexIter!=endOfResetListOfVertices ) {
    compareGenVertex(*origVertexIter,*resetVertexIter);
    ++origVertexIter;
    ++resetVertexIter;
  }
  return;
#endif
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
  HepMC::FourVector myPos( 0.0345682751, 0.00872347682, 0.23671987, 0.0 );
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
  HepMC::FourVector myPos(0.0054625871, 0.08027374862, 0.32769178, 0.0);
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
  McEventCollectionCnv_p6 cnv;
  McEventCollection_p6 pers;
  cnv.transToPers (&trans1, &pers, log);
  McEventCollection trans2;
  cnv.persToTrans (&pers, &trans2, log);
#if HEPMC3
  compare (trans1, trans2);
#else
  // TP conversion of HepMC2::GenEvents has a feature where the order
  // of GenParticles associated with each GenVertex is flipped, so
  // agreement is only restored after running TP conversion twice...
  McEventCollection_p6 pers2;
  cnv.transToPers (&trans2, &pers2, log);
  McEventCollection trans3;
  cnv.persToTrans (&pers2, &trans3, log);

  compare (trans1, trans3);
#endif
}

void test1()
{
  std::cout << "test1\n";

#ifdef HEPMC3
  auto runInfo = std::make_shared<HepMC3::GenRunInfo>();
  runInfo->set_weight_names ({"weight1"});
#endif

  McEventCollection trans1;
  // Add a dummy GenEvent
  const int process_id1(20);
  const int event_number1(17);
  trans1.push_back(HepMC::newGenEvent(process_id1, event_number1));
#ifdef HEPMC3
  trans1.back()->set_run_info (runInfo);
#endif
  HepMC::GenEvent& ge1 = *(trans1.at(0));
  populateGenEvent(ge1);
  // Add a second dummy GenEvent
  const int process_id2(20);
  const int event_number2(25);
  trans1.push_back(HepMC::newGenEvent(process_id2, event_number2));
#ifdef HEPMC3
  trans1.back()->set_run_info (runInfo);
#endif
  HepMC::GenEvent& ge2 = *(trans1.at(1));
  populateGenEvent2(ge2);

  testit (trans1);
}


int main()
{
  setlinebuf(stdout);
  setlinebuf(stderr);

  std::cout << "GeneratorObjectsTPCnv/McEventCollectionCnv_p6_test\n";
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("GeneratorObjectsTPCnv/GeneratorObjectsTPCnv_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  test1();
  return 0;
}
