/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTruthAlg.h"

#include "TrkTruthData/TrackTruthCollection.h"
#include "TrkTruthData/TrackTruthKey.h"

#include "GeneratorObjects/xAODTruthParticleLink.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "GeneratorObjects/McEventCollection.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"

TrackParticleTruthAlg::TrackParticleTruthAlg(const std::string &name,ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_truthClassifier("MCTruthClassifier/MCTruthClassifier")
{
  declareProperty("MCTruthClassifier",  m_truthClassifier);
  declareProperty("TrackTruthName",     m_truthName="TrackTruthCollection");
  declareProperty("TrackParticleName",  m_trackParticleName="InDetTrackParticles");
  declareProperty("xAODTruthLinkVector",m_truthLinkVecName="xAODTruthLinks");
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackParticleTruthAlg::initialize()
{
  ATH_CHECK(m_truthClassifier.retrieve());
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackParticleTruthAlg::finalize() {
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackParticleTruthAlg::execute() {

  xAOD::TrackParticleContainer* particles = 0;
  if(evtStore()->contains<xAOD::TrackParticleContainer>(m_trackParticleName)){
    ATH_CHECK(evtStore()->retrieve(particles, m_trackParticleName));
  }else return StatusCode::SUCCESS;


  const xAODTruthParticleLinkVector* truthParticleLinkVec = 0;
  if(evtStore()->contains<xAODTruthParticleLinkVector>(m_truthLinkVecName)){
    ATH_CHECK(evtStore()->retrieve(truthParticleLinkVec, m_truthLinkVecName));
  }else return StatusCode::SUCCESS;
  
  const TrackTruthCollection* truthTracks = 0;
  // Retrieve the input
  if( evtStore()->contains<TrackTruthCollection>(m_truthName)){
    ATH_CHECK(evtStore()->retrieve(truthTracks, m_truthName));
  }else return StatusCode::SUCCESS;

  for( auto particle : *particles ){
    
    if( !particle->trackLink().isValid() ){
      ATH_MSG_WARNING("Found TrackParticle with Invalid element link, skipping");
      continue;
    }
    ATH_MSG_DEBUG("Looking up truth for pt " << particle->pt() << " eta " << particle->eta() << " phi " << particle->phi());

    MCTruthPartClassifier::ParticleType type = MCTruthPartClassifier::Unknown;
    MCTruthPartClassifier::ParticleOrigin origin = MCTruthPartClassifier::NonDefined;
    ElementLink<xAOD::TruthParticleContainer> link;
    // look-up associdated truth particle
    Trk::TrackTruthKey key(particle->trackLink());
    auto result = truthTracks->find(key);
    
    // if we found a match use it
    if( result != truthTracks->end() ){
      ATH_MSG_VERBOSE("Found track Truth: barcode  " << result->second.particleLink().barcode() << " evt " << result->second.particleLink().eventIndex());
      link = truthParticleLinkVec->find(result->second.particleLink());
      
      // if configured also get truth classification
      if( link.isValid()&& !m_truthClassifier.empty() ){
	auto truthClass = m_truthClassifier->particleTruthClassifier(*link);
	type = truthClass.first;
	origin = truthClass.second;
	ATH_MSG_VERBOSE("Got truth type  " << static_cast<int>(type) << "  origin " << static_cast<int>(origin));
      }
    }
    
    ElementLink<xAOD::TruthParticleContainer>& theLink = particle->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" );
    if( link.isValid() ){
      ATH_MSG_DEBUG("Found matching xAOD Truth: barcode " << (*link)->barcode() << " pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi());
      // set element link 
      link.toPersistent();
      theLink = link;
    }
    if( !m_truthClassifier.empty() ){
      particle->auxdata<int>("truthType") = static_cast<int>(type);
      particle->auxdata<int>("truthOrigin") = static_cast<int>(origin);
    } 
  }
  return StatusCode::SUCCESS;
}
