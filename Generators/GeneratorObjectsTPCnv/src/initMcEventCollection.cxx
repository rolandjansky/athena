#include "GeneratorObjectsTPCnv/initMcEventCollection.h"

// HepMC includes
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "GeneratorObjectsTPCnv/HepMcParticleLinkCnv_p1.h"
#include "StoreGate/WriteHandle.h"
#include "GeneratorObjects/McEventCollection.h"

#include "TestTools/initGaudi.h"

namespace Athena_test {
  bool initMcEventCollection(ISvcLocator*& pSvcLoc, std::vector<HepMC::GenParticle*>& genPartList)
  {
    if (!Athena_test::initGaudi(pSvcLoc)) {
      std::cerr << "This test can not be run" << std::endl;
      return false;
    }
    // create dummy input McEventCollection with a name that
    // HepMcParticleLink knows about
    SG::WriteHandle<McEventCollection> inputTestDataHandle{"TruthEvent"};
    inputTestDataHandle = std::make_unique<McEventCollection>();
    // Add a dummy GenEvent
    const int process_id1(20);
    const int event_number1(17);
    inputTestDataHandle->push_back(new HepMC::GenEvent(process_id1, event_number1));
    HepMC::GenEvent& ge1 = *(inputTestDataHandle->at(0));
    populateGenEvent(ge1,-11,11,genPartList);
    populateGenEvent(ge1,-13,13,genPartList);
    populateGenEvent(ge1,-11,11,genPartList);
    populateGenEvent(ge1,-13,13,genPartList);
    populateGenEvent(ge1,-11,11,genPartList);
    populateGenEvent(ge1,22,22,genPartList);
    return true;
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
}
