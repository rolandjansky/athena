/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTruthAlg.h"


#include "TrkTruthData/TrackTruthKey.h"
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "GeneratorObjects/McEventCollection.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"

TrackParticleTruthAlg::TrackParticleTruthAlg(const std::string &name,ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_particles("InDetTrackParticles"),
  m_truthParticleLinkVec("xAODTruthLinks"),
  m_truthTracks("TrackTruthCollection"),
  m_truthClassifier("MCTruthClassifier/MCTruthClassifier")
{
  declareProperty("MCTruthClassifier",  m_truthClassifier);
  declareProperty("TrackTruthName",     m_truthTracks);
  declareProperty("TrackParticleName",  m_particles);
  declareProperty("xAODTruthLinkVector",m_truthParticleLinkVec);
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


  //if(!m_particles.isPresent()){ //no track particle container of this name is in SG
  if(!evtStore()->contains<xAOD::TrackParticleContainer>(m_particles.name())){ //isPresent doesn't seem to work right now so keep the old way
    ATH_MSG_WARNING("TrackParticleTruthAlg: "<<m_particles.name()<<" not found");
    return StatusCode::SUCCESS;
  }
  
  if(!m_particles.isValid()){
    ATH_MSG_ERROR("Could not read " << m_particles.name());
    return StatusCode::FAILURE;
  }

  if(!m_truthParticleLinkVec.isValid()){
    ATH_MSG_ERROR("Could not read " << m_truthParticleLinkVec.name());
    return StatusCode::FAILURE;
  }

  if(!m_truthTracks.isValid()){
    ATH_MSG_ERROR("Could not read " << m_truthTracks.name());
    return StatusCode::FAILURE;
  }


  for( auto particle : *m_particles ){
    
    if( !particle->trackLink().isValid() ){
      ATH_MSG_WARNING("Found TrackParticle with Invalid element link, skipping");
      //add dummy truth link
      const_cast<xAOD::TrackParticle*>(particle)->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" ) = ElementLink<xAOD::TruthParticleContainer>();
      continue;
    }
    ATH_MSG_DEBUG("Looking up truth for pt " << particle->pt() << " eta " << particle->eta() << " phi " << particle->phi());

    MCTruthPartClassifier::ParticleType type = MCTruthPartClassifier::Unknown;
    MCTruthPartClassifier::ParticleOrigin origin = MCTruthPartClassifier::NonDefined;
    ElementLink<xAOD::TruthParticleContainer> link;
    // look-up associdated truth particle
    Trk::TrackTruthKey key(particle->trackLink());
    auto result = m_truthTracks->find(key);
    
    // if we found a match use it
    if( result != m_truthTracks->end() ){
      ATH_MSG_VERBOSE("Found track Truth: barcode  " << result->second.particleLink().barcode() << " evt " << result->second.particleLink().eventIndex());
      link = m_truthParticleLinkVec->find(result->second.particleLink());
      
      // if configured also get truth classification
      if( link.isValid()&& !m_truthClassifier.empty() ){
	auto truthClass = m_truthClassifier->particleTruthClassifier(*link);
	type = truthClass.first;
	origin = truthClass.second;
	ATH_MSG_VERBOSE("Got truth type  " << static_cast<int>(type) << "  origin " << static_cast<int>(origin));
      }
    }
    
    
    if( link.isValid() ){
      ATH_MSG_DEBUG("Found matching xAOD Truth: barcode " << (*link)->barcode() << " pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi());
      // set element link 
      link.toPersistent();
      //BREAKS THREAD SAFETY FIX LATER
      const_cast<xAOD::TrackParticle*>(particle)->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" ) = link;
    }
    else{ //no truth link, add a dummy
      const_cast<xAOD::TrackParticle*>(particle)->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" ) = ElementLink<xAOD::TruthParticleContainer>();
    }
    if( !m_truthClassifier.empty() ){
      //BREAKS THREAD SAFETY FIX LATER
      const_cast<xAOD::TrackParticle*>(particle)->auxdata<int>("truthType") = static_cast<int>(type);
      const_cast<xAOD::TrackParticle*>(particle)->auxdata<int>("truthOrigin") = static_cast<int>(origin);
    } 
  }
  return StatusCode::SUCCESS;
}
