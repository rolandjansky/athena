/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedInDetExtensionAlg.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedInDetExtensionTool.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"


MuonCombinedInDetExtensionAlg::MuonCombinedInDetExtensionAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator)
{  
  declareProperty("MuonCombinedInDetExtensionTools",m_muonCombinedInDetExtensionTools);
  declareProperty("InDetCandidateLocation",m_indetCandidateCollectionName = "InDetCandidates" );
}

MuonCombinedInDetExtensionAlg::~MuonCombinedInDetExtensionAlg(){}

StatusCode MuonCombinedInDetExtensionAlg::initialize()
{

  ATH_CHECK(m_muonCombinedInDetExtensionTools.retrieve());
  ATH_CHECK(m_indetCandidateCollectionName.initialize());

  return StatusCode::SUCCESS; 
}

StatusCode MuonCombinedInDetExtensionAlg::execute()
{

  SG::ReadHandle<InDetCandidateCollection> indetCandidateCollection(m_indetCandidateCollectionName);
  if(!indetCandidateCollection.isValid()){
    ATH_MSG_ERROR("Could not read "<< m_indetCandidateCollectionName);
    return StatusCode::FAILURE;
  }

  for(auto& tool : m_muonCombinedInDetExtensionTools)
    tool->extend(*indetCandidateCollection);
  
  return StatusCode::SUCCESS;
}


StatusCode MuonCombinedInDetExtensionAlg::finalize()
{
  return StatusCode::SUCCESS;
}
