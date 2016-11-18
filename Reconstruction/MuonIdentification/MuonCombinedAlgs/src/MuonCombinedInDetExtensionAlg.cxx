/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedInDetExtensionAlg.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"

#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"


MuonCombinedInDetExtensionAlg::MuonCombinedInDetExtensionAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator)
{  
  declareProperty("MuonCombinedInDetExtensionTools",m_muonCombinedInDetExtensionTools);
  declareProperty("InDetCandidateLocation",m_indetCandidateCollectionName = "InDetCandidates" );
}

MuonCombinedInDetExtensionAlg::~MuonCombinedInDetExtensionAlg()
{

}

StatusCode MuonCombinedInDetExtensionAlg::initialize()
{

  ATH_CHECK(m_muonCombinedInDetExtensionTools.retrieve());

  return StatusCode::SUCCESS; 
}

StatusCode MuonCombinedInDetExtensionAlg::execute()
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

  for(auto& tool : m_muonCombinedInDetExtensionTools)
    tool->extend(*indetCandidateCollection);
  
  return StatusCode::SUCCESS;
}


StatusCode MuonCombinedInDetExtensionAlg::finalize()
{
  return StatusCode::SUCCESS;
}
