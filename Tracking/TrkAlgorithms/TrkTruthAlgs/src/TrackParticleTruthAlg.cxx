/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackParticleTruthAlg.h"


#include "TrkTruthData/TrackTruthKey.h"
#include "xAODCore/AuxStoreAccessorMacros.h"
#include "GeneratorObjects/McEventCollection.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "StoreGate/WriteDecorHandle.h"

TrackParticleTruthAlg::TrackParticleTruthAlg(const std::string &name,ISvcLocator *pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_particles("MuonSpectrometerTrackParticles"),
  m_particlesLinkKey("MuonSpectrometerTrackParticles"),
  m_particlesTypeKey("MuonSpectrometerTrackParticles"),
  m_particlesOriginKey("MuonSpectrometerTrackParticles"),
  m_particleName("InDetTrackParticles"),
  m_truthParticleLinkVecKey("xAODTruthLinks"),
  m_truthTracksKey("TrackTruthCollection"),
  m_truthClassifier("MCTruthClassifier/MCTruthClassifier")
{
  declareProperty("ParticleLinkKey",    m_particlesLinkKey);
  declareProperty("ParticleTypeKey",    m_particlesTypeKey);
  declareProperty("ParticleOriginKey",  m_particlesOriginKey);
  declareProperty("MCTruthClassifier",  m_truthClassifier);
  declareProperty("TrackTruthName",     m_truthTracksKey);
  declareProperty("TrackParticleName",  m_particleName);
  declareProperty("xAODTruthLinkVector",m_truthParticleLinkVecKey);
  declareProperty("InputParticles",     m_particles);
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackParticleTruthAlg::initialize()
{
  std::string keyName=m_particleName+".truthParticleLink";
  m_particlesLinkKey=keyName.c_str();
  m_particlesTypeKey=m_particleName+".truthType";
  m_particlesOriginKey=m_particleName+".truthOrigin";
  m_particles=m_particleName;
  ATH_CHECK(m_particlesLinkKey.initialize());
  ATH_CHECK(m_particlesTypeKey.initialize());
  ATH_CHECK(m_particlesOriginKey.initialize());
  ATH_CHECK(m_particles.initialize());
  ATH_CHECK(m_truthParticleLinkVecKey.initialize());
  ATH_CHECK(m_truthTracksKey.initialize());
  ATH_CHECK(m_truthClassifier.retrieve());
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackParticleTruthAlg::finalize() {
  return StatusCode::SUCCESS;
}

// -----------------------------------------------------------------------------------------------------
StatusCode TrackParticleTruthAlg::execute() {

  SG::ReadHandle<TrackTruthCollection> m_truthTracks(m_truthTracksKey);
  SG::ReadHandle<xAODTruthParticleLinkVector> m_truthParticleLinkVec(m_truthParticleLinkVecKey);
  SG::WriteDecorHandle<xAOD::TrackParticleContainer,ElementLink<xAOD::TruthParticleContainer> > m_particlesLink(m_particlesLinkKey);
  SG::WriteDecorHandle<xAOD::TrackParticleContainer,int> m_particlesType(m_particlesTypeKey);
  SG::WriteDecorHandle<xAOD::TrackParticleContainer,int> m_particlesOrigin(m_particlesOriginKey);
  //SG::ReadHandle<xAOD::TrackParticleContainer> m_particleHandle(m_particles);

  //if(!m_particleHandle.isPresent()) ATH_MSG_WARNING(m_particles.key()<<" not present");
  //else ATH_MSG_WARNING(m_particles.key()<<" present");

  if(!m_particlesLink.isPresent()){ //no track particle container of this name is in SG
    //if(!evtStore()->contains<xAOD::TrackParticleContainer>(m_particles.name())){ //isPresent doesn't seem to work right now so keep the old way
    std::cout<<"no m_particlesLink present"<<std::endl;
    ATH_MSG_WARNING("TrackParticleTruthAlg: "<<m_particlesLink.name()<<" not found");
    //return StatusCode::SUCCESS;
  }
  std::cout<<"checked if m_particlesLink present"<<std::endl;

  //if(!m_particleHandle.isValid()) ATH_MSG_WARNING("read handle not valid");
  //else ATH_MSG_WARNING("got read handle");

  if(!m_particlesLink.isValid()){
    std::cout<<"m_particlesLink not valid"<<std::endl;
    ATH_MSG_ERROR("Could not read " << m_particlesLink.name());
    return StatusCode::FAILURE;
  }
  std::cout<<"m_particles link valid"<<std::endl;

  if(!m_truthParticleLinkVec.isValid()){
    ATH_MSG_ERROR("Could not read " << m_truthParticleLinkVec.name());
    return StatusCode::FAILURE;
  }

  if(!m_truthTracks.isValid()){
    ATH_MSG_ERROR("Could not read " << m_truthTracks.name());
    return StatusCode::FAILURE;
  }

  const TrackTruthCollection truthTracks=*m_truthTracks;
  const xAODTruthParticleLinkVector truthParticleLinkVec=*m_truthParticleLinkVec;

  std::cout<<"loop over m_particlesLink"<<std::endl;
  int partInd=0;
  for( const auto particle : *m_particlesLink ){
    std::cout<<"track #"<<partInd<<std::endl;

    if( !particle->trackLink().isValid() ){
      ATH_MSG_WARNING("Found TrackParticle with Invalid element link, skipping");
      continue;
    }
    ATH_MSG_DEBUG("Looking up truth for pt " << particle->pt() << " eta " << particle->eta() << " phi " << particle->phi());
    std::cout<<"Looking up truth for pt " << particle->pt() << " eta " << particle->eta() << " phi " << particle->phi()<<std::endl;

    MCTruthPartClassifier::ParticleType type = MCTruthPartClassifier::Unknown;
    MCTruthPartClassifier::ParticleOrigin origin = MCTruthPartClassifier::NonDefined;
    ElementLink<xAOD::TruthParticleContainer> link;
    // look-up associdated truth particle

    Trk::TrackTruthKey key(particle->trackLink());
    auto result = truthTracks.find(key);
    
    // if we found a match use it
    if( result != truthTracks.end() ){
      ATH_MSG_VERBOSE("Found track Truth: barcode  " << result->second.particleLink().barcode() << " evt " << result->second.particleLink().eventIndex());
      std::cout<<"Found track Truth: barcode  " << result->second.particleLink().barcode() << " evt " << result->second.particleLink().eventIndex()<<std::endl;
      link = truthParticleLinkVec.find(result->second.particleLink());
      
      // if configured also get truth classification

      if( link.isValid()&& !m_truthClassifier.empty() ){
	auto truthClass = m_truthClassifier->particleTruthClassifier(*link);
	type = truthClass.first;
	origin = truthClass.second;
	ATH_MSG_VERBOSE("Got truth type  " << static_cast<int>(type) << "  origin " << static_cast<int>(origin));
	std::cout<<"Got truth type  " << static_cast<int>(type) << "  origin " << static_cast<int>(origin)<<std::endl;
      }
    }

    if( link.isValid() ){
      ATH_MSG_DEBUG("Found matching xAOD Truth: barcode " << (*link)->barcode() << " pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi());
      std::cout<<"Found matching xAOD Truth: barcode " << (*link)->barcode() << " pt " << (*link)->pt() << " eta " << (*link)->eta() << " phi " << (*link)->phi()<<std::endl;
      // set element link 
      link.toPersistent();
      //BREAKS THREAD SAFETY FIX LATER
      //const_cast<xAOD::TrackParticle*>(particle)->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" ) = link;
      m_particlesLink(*particle)=link;

    }
    else{ //no truth link, add a dummy
      m_particlesLink(*particle)=ElementLink<xAOD::TruthParticleContainer>();
    }

    if( !m_truthClassifier.empty() ){
      //use index for these since it's the same particle
      m_particlesType(partInd)=static_cast<int>(type);
      m_particlesOrigin(partInd)=static_cast<int>(origin);
      //const_cast<xAOD::TrackParticle*>(particle)->auxdata<int>("truthType") = static_cast<int>(type);
      //const_cast<xAOD::TrackParticle*>(particle)->auxdata<int>("truthOrigin") = static_cast<int>(origin);
    }
    partInd++;
  }
  return StatusCode::SUCCESS;
}
