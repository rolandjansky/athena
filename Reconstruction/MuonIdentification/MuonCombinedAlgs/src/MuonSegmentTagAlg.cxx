/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentTagAlg.h"
#include "xAODMuon/MuonSegmentContainer.h"

MuonSegmentTagAlg::MuonSegmentTagAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator) {  
}

StatusCode MuonSegmentTagAlg::initialize()
{
  
  ATH_CHECK(m_muonSegmentTagTool.retrieve());
  ATH_CHECK(m_indetCandidateCollectionName.initialize());
  ATH_CHECK(m_muonSegmentCollectionName.initialize());
  ATH_CHECK(m_tagMap.initialize());
  
  return StatusCode::SUCCESS; 
}

StatusCode MuonSegmentTagAlg::execute()
{

  SG::ReadHandle<InDetCandidateCollection> indetCandidateCollection(m_indetCandidateCollectionName);
  if(!indetCandidateCollection.isValid()){
    ATH_MSG_ERROR("Could not read "<< m_indetCandidateCollectionName);
    return StatusCode::FAILURE;
  }
  if(!indetCandidateCollection.isPresent()){
    ATH_MSG_WARNING(m_indetCandidateCollectionName<<" not found in StoreGate");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<xAOD::MuonSegmentContainer>segments(m_muonSegmentCollectionName);
  if(!segments.isPresent()){
    ATH_MSG_WARNING(m_muonSegmentCollectionName<<" not found in StoreGate");
    return StatusCode::SUCCESS;
  }
  if(!segments.isValid()){
    ATH_MSG_FATAL( "Unable to retrieve " << m_muonSegmentCollectionName );
    return StatusCode::FAILURE;
  }

  SG::WriteHandle<MuonCombined::InDetCandidateToTagMap> tagMap(m_tagMap);
  ATH_CHECK( tagMap.record (std::make_unique<MuonCombined::InDetCandidateToTagMap>()) );

  m_muonSegmentTagTool->tag(*indetCandidateCollection,*segments,tagMap.ptr());
  
  return StatusCode::SUCCESS;
}
