/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentTagAlg.h"
#include "MuonCombinedToolInterfaces/IMuonSegmentTagTool.h"

#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "xAODMuon/MuonSegmentContainer.h"

MuonSegmentTagAlg::MuonSegmentTagAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_muonSegmentTagTool("MuonCombined::MuonSegmentTagTool/MuonSegmentTagTool")
{  
  declareProperty("MuonSegmentTagTool",m_muonSegmentTagTool);
  declareProperty("InDetCandidateLocation",m_indetCandidateCollectionName = "InDetCandidates" );
  declareProperty("MuonSegmentLocation", m_muonSegmenteCollectionName = "MuonSegments" );
}

MuonSegmentTagAlg::~MuonSegmentTagAlg()
{

}

StatusCode MuonSegmentTagAlg::initialize()
{
  
  ATH_CHECK(m_muonSegmentTagTool.retrieve());

  return StatusCode::SUCCESS; 
}

StatusCode MuonSegmentTagAlg::execute()
{

  InDetCandidateCollection* indetCandidateCollection = 0;
  if(evtStore()->contains<InDetCandidateCollection>(m_indetCandidateCollectionName)) {
    if(evtStore()->retrieve(indetCandidateCollection,m_indetCandidateCollectionName).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_indetCandidateCollectionName );
      return StatusCode::FAILURE;
    }
  }
  
  if( !indetCandidateCollection ){
    ATH_MSG_WARNING("InDetCandidates not found in StoreGate");
    return StatusCode::SUCCESS;
  }

  xAOD::MuonSegmentContainer* segments = 0;
  if(evtStore()->contains<xAOD::MuonSegmentContainer>(m_muonSegmenteCollectionName)) {
    if(evtStore()->retrieve(segments,m_muonSegmenteCollectionName).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_muonSegmenteCollectionName );
      return StatusCode::FAILURE;
    }
  }
  
  if( !segments ){
    ATH_MSG_WARNING("MuonSegments not found in StoreGate");
    return StatusCode::SUCCESS;
  }

  m_muonSegmentTagTool->tag(*indetCandidateCollection,*segments);
  
  return StatusCode::SUCCESS;
}


StatusCode MuonSegmentTagAlg::finalize()
{
  return StatusCode::SUCCESS;
}
