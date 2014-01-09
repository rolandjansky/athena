/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EmbeddedTrackGenerator generates particles "embedded" in reconstructed jets.
// The jets are taken from an AOD file.
//
// Andrei Gaponenko <andrei.gaponenko@cern.ch>, 2006-2008
//

#include "EmbeddedTrackGenerator/EmbeddedTrackGenerator.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"

#include "VxVertex/RecVertex.h"

#include <cmath>
#include <cstdio>

#include "GaudiKernel/IEvtSelector.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


//--------------------------------------------------------------------------


EmbeddedTrackGenerator::EmbeddedTrackGenerator(const std::string& name, ISvcLocator* pSvcLocator)
  : EmbeddedGeneratorCommon(name,pSvcLocator),
    GenPDG_Property(this)
{
  declareProperty("jetCollectionName",  m_jetName = "Cone4H1TowerJets");
  declareProperty("maxParticles",  m_maxParticles = 1);
  declareProperty("ptot",  m_ptot = 10.*CLHEP::GeV);
  declareProperty("dR_max",  m_dR_max = 0.4);
  declareProperty("displacement_max",  m_displacementMax = 0.);
}


//--------------------------------------------------------------------------


StatusCode EmbeddedTrackGenerator::embeddedGenInitialize() {
  return GenPDG_Property::initialize(&particleTable());
}


//---------------------------------------------------------------------------


StatusCode EmbeddedTrackGenerator::embeddedFillEvt(HepMC::GenEvent* a_event) {
  const JetCollection* jets(0);
  if (! (m_pileUpStream.store().retrieve(jets, m_jetName).isSuccess()) ) {
    ATH_MSG_ERROR("Could not get jet container");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Got " << jets->size() << " jets");

  unsigned num_generated = 0;
  for (JetCollection::const_iterator i=jets->begin();
       (i!=jets->end()) && (num_generated < m_maxParticles);
       ++i, ++num_generated) {

    const Jet *jet = *i;
    if (jet) {
      const Trk::RecVertex *v = jet->origin();
      ATH_MSG_VERBOSE("jet: "<< (jet->hlv()) << (v ? ", YES origin " :  ", NO origin ") );
      if (v) {
        ParticleDescription desc = getParticle(&randomEngine());

        // Generate vertex smearing offsets
	//        CLHEP::Hep3Vector dr;
	Amg::Vector3D dr;
        do {
          double rnd[3];
          randomEngine().flatArray(3, rnd);
          for(unsigned i = 0; i < 3; ++i) {
            dr[i] = 2*m_displacementMax*(rnd[i]-0.5);
          }
        } while( std::pow(m_displacementMax,2) < dr.mag2() ); //m_displacementMax==0 is safe here
	Amg::Vector3D drv;
        drv=dr+v->position();
        HepMC::GenVertex* vertex(new HepMC::GenVertex(CLHEP::HepLorentzVector(drv(0),drv(1),drv(2), 0.) ));

        // Smear the direction.
        double eta0 = jet->eta();
        double phi0 = jet->phi();

        double eta, phi;
        do {
          double rnd[2];
          randomEngine().flatArray(2, rnd);
          eta = eta0 + m_dR_max * 2 * (rnd[0]-0.5);
          phi = phi0 + m_dR_max * 2 * (rnd[1]-0.5);
        } while( std::pow(m_dR_max, 2) < std::pow(eta-eta0, 2) + std::pow(phi-phi0, 2) );

        double energy = std::sqrt(std::pow(m_ptot,2) + std::pow(desc.second,2));

        CLHEP::HepLorentzVector momentum;
        momentum.setREtaPhi(m_ptot, eta, phi);
        momentum.setE(energy);
        ATH_MSG_DEBUG("momentum = " << momentum);

        HepMC::GenParticle* particle = new HepMC::GenParticle(momentum, desc.first, 1);

        a_event->add_vertex(vertex);
        vertex->add_particle_out(particle);
      }
      else {
        ATH_MSG_WARNING("No origin info for the jet, skipping.");
      }

    }
    else {
      ATH_MSG_WARNING("Invalid entry in the jet container!");
    }
  } // end loop over jets

  return StatusCode::SUCCESS;
}
