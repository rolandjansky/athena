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
  declareProperty("usePRDs",m_usePRDs=false);
  declareProperty("useNSW", m_useNSW=false);
}

MuonCombinedInDetExtensionAlg::~MuonCombinedInDetExtensionAlg(){}

StatusCode MuonCombinedInDetExtensionAlg::initialize()
{
  ATH_CHECK(m_muonCombinedInDetExtensionTools.retrieve());
  ATH_CHECK(m_indetCandidateCollectionName.initialize());
  ATH_CHECK(m_MDT_ContainerName.initialize(m_usePRDs));
  ATH_CHECK(m_RPC_ContainerName.initialize(m_usePRDs));
  ATH_CHECK(m_TGC_ContainerName.initialize(m_usePRDs));
  ATH_CHECK(m_CSC_ContainerName.initialize(m_usePRDs && !m_useNSW));
  ATH_CHECK(m_sTGC_ContainerName.initialize(m_usePRDs && m_useNSW));
  ATH_CHECK(m_MM_ContainerName.initialize(m_usePRDs && m_useNSW));

  return StatusCode::SUCCESS; 
}

StatusCode MuonCombinedInDetExtensionAlg::execute()
{

  SG::ReadHandle<InDetCandidateCollection> indetCandidateCollection(m_indetCandidateCollectionName);
  if(!indetCandidateCollection.isValid()){
    ATH_MSG_ERROR("Could not read "<< m_indetCandidateCollectionName);
    return StatusCode::FAILURE;
  }

  if(m_usePRDs){
    SG::ReadHandle<Muon::MdtPrepDataContainer> mdtPRDContainer(m_MDT_ContainerName);
    SG::ReadHandle<Muon::CscPrepDataContainer> cscPRDContainer(m_CSC_ContainerName);
    SG::ReadHandle<Muon::RpcPrepDataContainer> rpcPRDContainer(m_RPC_ContainerName);
    SG::ReadHandle<Muon::TgcPrepDataContainer> tgcPRDContainer(m_TGC_ContainerName);
    if (m_useNSW) {
      SG::ReadHandle<Muon::sTgcPrepDataContainer> stgcPRDContainer(m_sTGC_ContainerName);
      SG::ReadHandle<Muon::MMPrepDataContainer> mmPRDContainer(m_MM_ContainerName);
      for(auto& tool : m_muonCombinedInDetExtensionTools)
        tool->extendWithPRDs(*indetCandidateCollection,mdtPRDContainer.cptr(),cscPRDContainer.cptr(),rpcPRDContainer.cptr(),tgcPRDContainer.cptr(),stgcPRDContainer.cptr(),mmPRDContainer.cptr());
     } else {
       for(auto& tool : m_muonCombinedInDetExtensionTools)
         tool->extendWithPRDs(*indetCandidateCollection,mdtPRDContainer.cptr(),cscPRDContainer.cptr(),rpcPRDContainer.cptr(),tgcPRDContainer.cptr(),0,0);
     }
  } else{
    for(auto& tool : m_muonCombinedInDetExtensionTools)
      tool->extend(*indetCandidateCollection);
  }
  
  return StatusCode::SUCCESS;
}


StatusCode MuonCombinedInDetExtensionAlg::finalize()
{
  return StatusCode::SUCCESS;
}
