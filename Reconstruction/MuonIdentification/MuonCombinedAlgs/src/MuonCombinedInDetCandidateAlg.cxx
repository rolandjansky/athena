/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedInDetCandidateAlg.h"
#include "MuonCombinedToolInterfaces/IInDetCandidateTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"


MuonCombinedInDetCandidateAlg::MuonCombinedInDetCandidateAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_indetCandidateTool("MuonCombined::InDetCandidateTool/InDetCandidateTool")
{  
  declareProperty("InDetCandidateTool",m_indetCandidateTool);
  declareProperty("TrackParticleLocation",m_indetTrackParticleLocation = "InDetTrackParticles");
  declareProperty("InDetCandidateLocation",m_candidateCollectionName = "InDetCandidates" );
}

MuonCombinedInDetCandidateAlg::~MuonCombinedInDetCandidateAlg()
{

}

StatusCode MuonCombinedInDetCandidateAlg::initialize()
{
  ATH_CHECK(m_indetCandidateTool.retrieve());

  return StatusCode::SUCCESS; 
}

StatusCode MuonCombinedInDetCandidateAlg::execute()
{
  // retrieve MuonSpectrometer tracks
  const xAOD::TrackParticleContainer* indetTrackParticles = 0;
  if(evtStore()->contains<xAOD::TrackParticleContainer>(m_indetTrackParticleLocation)) {
    if(evtStore()->retrieve(indetTrackParticles,m_indetTrackParticleLocation).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_indetTrackParticleLocation );
      return StatusCode::FAILURE;
    }
  }
  
  if( !indetTrackParticles ){ 
    ATH_MSG_WARNING("InDetTrackParticles tracks not found in StoreGate");
    return StatusCode::SUCCESS;
  }

  InDetCandidateCollection* indetCandidateCollection = new InDetCandidateCollection();
  m_indetCandidateTool->create(*indetTrackParticles,*indetCandidateCollection);
  
  if(evtStore()->record(indetCandidateCollection,m_candidateCollectionName).isFailure()) {
    ATH_MSG_FATAL( "Could not record " << m_candidateCollectionName );
    return StatusCode::FAILURE;
  }    

  return StatusCode::SUCCESS;
}


StatusCode MuonCombinedInDetCandidateAlg::finalize()
{
  return StatusCode::SUCCESS;
}
