/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentTagAlg.h"
#include "MuonCombinedToolInterfaces/IMuonSegmentTagTool.h"
#include "xAODMuon/MuonSegmentContainer.h"

MuonSegmentTagAlg::MuonSegmentTagAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_muonSegmentTagTool("MuonCombined::MuonSegmentTagTool/MuonSegmentTagTool")
{  
  declareProperty("MuonSegmentTagTool",m_muonSegmentTagTool);
  declareProperty("InDetCandidateLocation",m_indetCandidateCollectionName = "InDetCandidates" );
  declareProperty("MuonSegmentLocation", m_muonSegmenteCollectionName = "MuonSegments" );
}

MuonSegmentTagAlg::~MuonSegmentTagAlg(){}

StatusCode MuonSegmentTagAlg::initialize()
{
  
  ATH_CHECK(m_muonSegmentTagTool.retrieve());
  ATH_CHECK(m_indetCandidateCollectionName.initialize());
  
  return StatusCode::SUCCESS; 
}

StatusCode MuonSegmentTagAlg::execute()
{

  SG::ReadHandle<InDetCandidateCollection> indetCandidateCollection(m_indetCandidateCollectionName);
  if(!indetCandidateCollection.isValid()){
    ATH_MSG_ERROR("Could not read "<< m_indetCandidateCollectionName);
    return StatusCode::FAILURE;
  }

  const xAOD::MuonSegmentContainer* segments = 0;
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
