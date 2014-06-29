/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedMuonCandidateAlg.h"
#include "MuonCombinedToolInterfaces/IMuonCandidateTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"

MuonCombinedMuonCandidateAlg::MuonCombinedMuonCandidateAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_muonCandidateTool("MuonCombined::MuonCandidateTool/MuonCandidateTool")
{  
  declareProperty("MuonCandidateTool",m_muonCandidateTool);
  declareProperty("MuonSpectrometerTrackPArticleLocation",m_muonTrackParticleLocation = "MuonSpectrometerTrackParticles");
  declareProperty("MuonCandidateLocation",m_candidateCollectionName = "MuonCandidates" );
}

MuonCombinedMuonCandidateAlg::~MuonCombinedMuonCandidateAlg()
{

}

StatusCode MuonCombinedMuonCandidateAlg::initialize()
{

  return StatusCode::SUCCESS; 
}

StatusCode MuonCombinedMuonCandidateAlg::execute()
{

  // retrieve MuonSpectrometer tracks
  const xAOD::TrackParticleContainer* muonTrackParticles = 0;
  if(evtStore()->contains<xAOD::TrackParticleContainer>(m_muonTrackParticleLocation)) {
    if(evtStore()->retrieve(muonTrackParticles,m_muonTrackParticleLocation).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_muonTrackParticleLocation );
      return StatusCode::FAILURE;
    }
  }
  
  if( !muonTrackParticles ){ 
    ATH_MSG_WARNING("MuonTrackParticles tracks not found in StoreGate");
    return StatusCode::SUCCESS;
  }

  MuonCandidateCollection* muonCandidates = new MuonCandidateCollection();
  m_muonCandidateTool->create(*muonTrackParticles,*muonCandidates);
  if(evtStore()->record(muonCandidates,m_candidateCollectionName).isFailure()) {
    ATH_MSG_FATAL( "Could not record " << m_candidateCollectionName );
    return StatusCode::FAILURE;
  }    

  return StatusCode::SUCCESS;
}


StatusCode MuonCombinedMuonCandidateAlg::finalize()
{
  return StatusCode::SUCCESS;
}
