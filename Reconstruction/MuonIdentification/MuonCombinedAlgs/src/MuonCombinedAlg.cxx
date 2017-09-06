/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedAlg.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTool.h"


MuonCombinedAlg::MuonCombinedAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_muonCombinedTool("MuonCombined::MuonCombinedTool/MuonCombinedTool")
{  
  declareProperty("MuonCombinedTool",m_muonCombinedTool);
}

MuonCombinedAlg::~MuonCombinedAlg(){}

StatusCode MuonCombinedAlg::initialize()
{
  ATH_CHECK(m_muonCombinedTool.retrieve());
  ATH_CHECK(m_indetCandidateCollectionName.initialize());
  ATH_CHECK(m_muonCandidateCollectionName.initialize());
  return StatusCode::SUCCESS; 
}

StatusCode MuonCombinedAlg::execute()
{

  SG::ReadHandle<InDetCandidateCollection> indetCandidateCollection(m_indetCandidateCollectionName);
  if(!indetCandidateCollection.isValid()){
    ATH_MSG_ERROR("Could not read "<< m_indetCandidateCollectionName);
    return StatusCode::FAILURE;
  }
  if(!indetCandidateCollection.isPresent()){
    ATH_MSG_WARNING(m_indetCandidateCollectionName<<" not present");
    return StatusCode::SUCCESS;
  }

  SG::ReadHandle<MuonCandidateCollection> muonCandidateCollection(m_muonCandidateCollectionName);
  if(!muonCandidateCollection.isValid()){
    ATH_MSG_ERROR("Could not read "<< m_muonCandidateCollectionName);
    return StatusCode::FAILURE;
  }
  if(!muonCandidateCollection.isPresent()){
    ATH_MSG_WARNING(m_muonCandidateCollectionName<<" not present");
    return StatusCode::SUCCESS;
  }
  
  m_muonCombinedTool->combine(*muonCandidateCollection,*indetCandidateCollection);
  return StatusCode::SUCCESS;
}


StatusCode MuonCombinedAlg::finalize()
{
  return StatusCode::SUCCESS;
}
