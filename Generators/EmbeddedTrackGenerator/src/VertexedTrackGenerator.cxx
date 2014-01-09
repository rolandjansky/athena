/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VertexedTrackGenerator generates particles "vertexed" in reconstructed jets.
// The jets are taken from an AOD file.
//
// Andrei Gaponenko <andrei.gaponenko@cern.ch>, 2006-2008
//

#include "EmbeddedTrackGenerator/VertexedTrackGenerator.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "HepPDT/ParticleData.hh"
#include "HepPDT/ParticleDataTable.hh"

#include "VxVertex/RecVertex.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxContainer.h"

#include <cmath>
#include <cstdio>

#include "GaudiKernel/IEvtSelector.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


//--------------------------------------------------------------------------


VertexedTrackGenerator::VertexedTrackGenerator(const std::string& name, ISvcLocator* pSvcLocator)
  : EmbeddedGeneratorCommon(name,pSvcLocator),
    GenPDG_Property(this)
{
  declareProperty("vertexCollectionName",  m_vertexCollectionName = "VxPrimaryCandidate");
  declareProperty("particlesPerVertex",  m_particlesPerVertex = 1);
  declareProperty("maxVertexes",  m_maxVertexes = 1);

  declareProperty("ptMin",  m_ptMin = 10.*CLHEP::GeV);
  declareProperty("ptMax",  m_ptMax = 10.*CLHEP::GeV);

  declareProperty("etaMin",  m_etaMin = -3.);
  declareProperty("etaMax",  m_etaMax = +3.);

  declareProperty("phiMin",  m_phiMin = -M_PI);
  declareProperty("phiMax",  m_phiMax = +M_PI);
}


//--------------------------------------------------------------------------


StatusCode VertexedTrackGenerator::embeddedGenInitialize() {
  return GenPDG_Property::initialize(&particleTable());
}


//---------------------------------------------------------------------------


StatusCode VertexedTrackGenerator::embeddedFillEvt(HepMC::GenEvent* a_event) {
  const VxContainer* vertexes(0);
  if (! (m_pileUpStream.store().retrieve(vertexes, m_vertexCollectionName).isSuccess()) ) {
    ATH_MSG_ERROR("Could not get jet container");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Got " << vertexes->size() << " vertices");

  for (unsigned ivertex = 0; ivertex < std::min(static_cast<size_t>(m_maxVertexes), vertexes->size()); ++ivertex) {
    //    CLHEP::Hep3Vector pos = vertexes->at(ivertex)->recVertex().position();
    Eigen::Vector3d pos = vertexes->at(ivertex)->recVertex().position(); 
    ATH_MSG_VERBOSE("ivertex = " << ivertex << ", pos = " << pos);
    //    HepMC::GenVertex* vertex(new HepMC::GenVertex(CLHEP::HepLorentzVector(pos, 0.)));
    HepMC::GenVertex* vertex(new HepMC::GenVertex(CLHEP::HepLorentzVector(pos(0),pos(1),pos(2), 0.)));
     

    ParticleDescription desc = getParticle(&randomEngine());
    ATH_MSG_VERBOSE("PDG ID = " << desc.first << ", mass = " << desc.second);

    double rnd[3];
    randomEngine().flatArray(3, rnd);

    double pt  = m_ptMin  + rnd[0] *  (m_ptMax - m_ptMin);
    double eta = m_etaMin + rnd[1] *  (m_etaMax - m_etaMin);
    double phi = m_phiMin + rnd[2] *  (m_phiMax - m_phiMin);

    ATH_MSG_VERBOSE("pt = " << pt << ", eta = " << eta << ", phi = " << phi);

    // CLHEP's setRhoPhiEta() computes pz = pt/tan(theta) that may be
    // problematic at eta=0.  We'll use a safer way that does not rely
    // on FPE settings, to allow division by infinity.
    double theta(2 * atan ( exp (-eta) ));

    double ptot2 = std::pow(pt,2) * (1. + std::pow(cos(theta)/sin(theta), 2) );

    double energy = std::sqrt(ptot2 + std::pow(desc.second,2));

    CLHEP::HepLorentzVector momentum;
    momentum.setREtaPhi(sqrt(ptot2), eta, phi);
    momentum.setE(energy);
    ATH_MSG_VERBOSE("Momentum = " << momentum);

    HepMC::GenParticle* particle = new HepMC::GenParticle(momentum, desc.first, 1);
    a_event->add_vertex(vertex);
    vertex->add_particle_out(particle);

  } // end loop over vertexes

  return StatusCode::SUCCESS;
}
