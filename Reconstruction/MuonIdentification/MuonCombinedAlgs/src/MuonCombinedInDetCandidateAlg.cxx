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
  declareProperty("InDetForwardCandidateTool",m_indetForwardCandidateTool);
  declareProperty("TrackParticleLocation",m_indetTrackParticleLocation = "InDetTrackParticles");
  declareProperty("ForwardParticleLocation",m_indetForwardTrackParticleLocation = "InDetForwardTrackParticles");
  declareProperty("InDetCandidateLocation",m_candidateCollectionName = "InDetCandidates" );
  declareProperty("DoSiliconAssocForwardMuons",m_doSiliconForwardMuons=false);
}

MuonCombinedInDetCandidateAlg::~MuonCombinedInDetCandidateAlg()
{

}

StatusCode MuonCombinedInDetCandidateAlg::initialize()
{
  ATH_CHECK(m_indetCandidateTool.retrieve());
  if (m_doSiliconForwardMuons) {
    ATH_CHECK(m_indetForwardCandidateTool.retrieve());
  }
  return StatusCode::SUCCESS; 
}

StatusCode MuonCombinedInDetCandidateAlg::execute()
{

  // retrieve MuonSpectrometer tracks
  std::vector<ToolHandle<MuonCombined::IInDetCandidateTool> > canTool;
  std::vector<std::string> trackParticleLoc;

  canTool.push_back(m_indetCandidateTool);
  trackParticleLoc.push_back(m_indetTrackParticleLocation);

  if (m_doSiliconForwardMuons){
    canTool.push_back(m_indetForwardCandidateTool);
    trackParticleLoc.push_back(m_indetForwardTrackParticleLocation);
  }


  //output collection
  InDetCandidateCollection* indetCandidateCollection = NULL ; 

  for (unsigned int i(0);i<canTool.size();++i){

    const xAOD::TrackParticleContainer* indetTrackParticles = 0;
    if(evtStore()->contains<xAOD::TrackParticleContainer>(trackParticleLoc[i])){
      if(evtStore()->retrieve(indetTrackParticles,trackParticleLoc[i]).isFailure()) {
        ATH_MSG_FATAL( "Unable to retrieve " << trackParticleLoc[i] );
        return StatusCode::FAILURE;
      }
    }

    if( !indetTrackParticles ){ 
      continue;
    }

    InDetCandidateCollection* tempCandidates = new InDetCandidateCollection(SG::VIEW_ELEMENTS);
    canTool[i]->create(*indetTrackParticles,*tempCandidates);

    //Copy indetCandidateCollection into output collection
    if (!indetCandidateCollection) indetCandidateCollection = new InDetCandidateCollection(SG::OWN_ELEMENTS);
    indetCandidateCollection->insert(indetCandidateCollection->end(), tempCandidates->begin(), tempCandidates->end());

    delete tempCandidates;
  }

  if(!indetCandidateCollection){
    ATH_MSG_WARNING("InDetTrackParticles tracks not found in StoreGate");
    return StatusCode::SUCCESS;
  }

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
