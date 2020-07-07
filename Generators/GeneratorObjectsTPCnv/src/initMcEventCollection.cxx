/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "GeneratorObjectsTPCnv/initMcEventCollection.h"

// HepMC includes
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"
#include "StoreGate/WriteHandle.h"
#include "GeneratorObjects/McEventCollection.h"

#include "TestTools/initGaudi.h"

namespace Athena_test {
  bool initMcEventCollection(ISvcLocator*& pSvcLoc, std::vector<HepMC::GenParticlePtr>& genPartList, const bool initGaudi)
  {
    if (initGaudi && !Athena_test::initGaudi(pSvcLoc)) {
      std::cerr << "This test can not be run" << std::endl;
      return false;
    }
    // create dummy input McEventCollection with a name that
    // HepMcParticleLink knows about
    SG::WriteHandle<McEventCollection> inputTestDataHandle{"TruthEvent"};
    inputTestDataHandle = std::make_unique<McEventCollection>();

    // create a dummy EventContext
    EventContext ctx;
    ctx.setExtension( Atlas::ExtendedEventContext( SG::CurrentEventStore::store() ) );
    Gaudi::Hive::setCurrentContext( ctx );

    // Add a dummy GenEvent
    const int process_id1(20);
    const int event_number1(17);
    inputTestDataHandle->push_back(HepMC::newGenEvent(process_id1, event_number1));
    HepMC::GenEvent& ge1 = *(inputTestDataHandle->at(0));
    populateGenEvent(ge1,-11,11,genPartList);
    populateGenEvent(ge1,-13,13,genPartList);
    populateGenEvent(ge1,-11,11,genPartList);
    populateGenEvent(ge1,-13,13,genPartList);
    populateGenEvent(ge1,-11,11,genPartList);
    populateGenEvent(ge1,22,22,genPartList);
    return true;
  }

  void populateGenEvent(HepMC::GenEvent & ge, int pdgid1, int pdgid2, std::vector<HepMC::GenParticlePtr>& genPartList)
  {
    HepMC::FourVector myPos( 0.0, 0.0, 0.0, 0.0);
    HepMC::GenVertexPtr myVertex = HepMC::newGenVertexPtr( myPos, -1 );
    HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticlePtr inParticle1 = HepMC::newGenParticlePtr(fourMomentum1, pdgid1, 2);
    myVertex->add_particle_in(inParticle1);
    HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticlePtr inParticle2 = HepMC::newGenParticlePtr(fourMomentum2, pdgid2, 2);
    myVertex->add_particle_in(inParticle2);
    HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticlePtr inParticle3 = HepMC::newGenParticlePtr(fourMomentum3, pdgid1, 1);
    myVertex->add_particle_out(inParticle3);
    genPartList.push_back(inParticle3);
    HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticlePtr inParticle4 = HepMC::newGenParticlePtr(fourMomentum4, pdgid2, 1);
    myVertex->add_particle_out(inParticle4);
    genPartList.push_back(inParticle4);
    ge.add_vertex( myVertex );
    HepMC::set_signal_process_vertex(&ge, myVertex );
    ge.set_beam_particles(inParticle1,inParticle2);
  }
}
