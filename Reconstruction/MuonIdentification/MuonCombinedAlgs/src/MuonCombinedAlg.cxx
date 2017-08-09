/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedAlg.h"
#include "MuonCombinedToolInterfaces/IMuonCombinedTool.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"


MuonCombinedAlg::MuonCombinedAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_muonCombinedTool("MuonCombined::MuonCombinedTool/MuonCombinedTool")
{  
  declareProperty("MuonCombinedTool",m_muonCombinedTool);
  declareProperty("InDetCandidateLocation",m_indetCandidateCollectionName = "InDetCandidates" );
  declareProperty("MuonCandidateLocation", m_muonCandidateCollectionName = "MuonCandidates" );
}

MuonCombinedAlg::~MuonCombinedAlg(){}

StatusCode MuonCombinedAlg::initialize()
{
  ATH_CHECK(m_muonCombinedTool.retrieve());
  ATH_CHECK(m_indetCandidateCollectionName.initialize());
  return StatusCode::SUCCESS; 
}

StatusCode MuonCombinedAlg::execute()
{

  SG::ReadHandle<InDetCandidateCollection> indetCandidateCollection(m_indetCandidateCollectionName);
  if(!indetCandidateCollection.isValid()){
    ATH_MSG_ERROR("Could not read "<< m_indetCandidateCollectionName);
    return StatusCode::FAILURE;
  }
  
  MuonCandidateCollection* muonCandidateCollection = 0;
  if(evtStore()->contains<MuonCandidateCollection>(m_muonCandidateCollectionName)) {
    if(evtStore()->retrieve(muonCandidateCollection,m_muonCandidateCollectionName).isFailure()) {
      ATH_MSG_FATAL( "Unable to retrieve " << m_muonCandidateCollectionName );
      return StatusCode::FAILURE;
    }
  }
  
  if( !muonCandidateCollection ){
    ATH_MSG_WARNING("MuonCandidates not found in StoreGate");
    return StatusCode::SUCCESS;
  }
  m_muonCombinedTool->combine(*muonCandidateCollection,*indetCandidateCollection);
  return StatusCode::SUCCESS;
}


StatusCode MuonCombinedAlg::finalize()
{
  return StatusCode::SUCCESS;
}
