/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LUCID_RawDataByteStreamCnv/LUCID_ByteStreamRawDataCnv.h"
using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

LUCID_ByteStreamRawDataCnv::LUCID_ByteStreamRawDataCnv(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm     (name                , pSvcLocator),
  m_storeGate      ("StoreGateSvc"      , name),
  m_robDataProvider("ROBDataProviderSvc", name) {
  
  declareProperty("lucid_RawDataContainerKey", m_lucid_RawDataContainerKey = "Lucid_RawData");
}

LUCID_ByteStreamRawDataCnv::~LUCID_ByteStreamRawDataCnv(){}

StatusCode LUCID_ByteStreamRawDataCnv::initialize() {
  
  ATH_MSG_INFO("LUCID_ByteStreamRawDataCnv::initialize");

  StatusCode sc = m_robDataProvider.retrieve();
  
  if (sc.isFailure()) ATH_MSG_WARNING(" Could not retrieve ROBDataProviderSvc ");
  else                ATH_MSG_DEBUG  (" Retrieved service ROBDataProviderSvc ");
  
  return StatusCode::SUCCESS;
}

StatusCode LUCID_ByteStreamRawDataCnv::execute() {
  
  ATH_MSG_DEBUG(" LUCID_ByteStreamRawDataCnv::execute ");
  
  std::vector<const ROBFragment*> listOfRobf;
  std::vector<unsigned int> ROBIDs;
  
  ROBIDs.push_back(m_rodDecoder.getSourceID());
  
  m_robDataProvider->getROBData(ROBIDs, listOfRobf);
  
  m_LUCID_RawDataContainer = new LUCID_RawDataContainer();
  
  StatusCode sc = fillContainer(listOfRobf);
  
  if (sc.isFailure()) ATH_MSG_WARNING(" fillContainer failed ");
  else                ATH_MSG_DEBUG  (" fillContainer success ");

  sc = m_storeGate->record(m_LUCID_RawDataContainer, m_lucid_RawDataContainerKey);
  
  if (sc.isFailure()) ATH_MSG_WARNING(" Could not record LUCID_RawDataContainer in StoreGate ");
  else                ATH_MSG_DEBUG  (" LUCID_RawDataContainer is recorded in StoreGate ");
  
  return StatusCode::SUCCESS;
}

StatusCode LUCID_ByteStreamRawDataCnv::fillContainer(std::vector<const ROBFragment*> listOfRobf) {
  
  ATH_MSG_DEBUG(" LUCID_ByteStreamRawDataCnv::fillContainer ");
  
  int nLucidFragments = listOfRobf.size();
    
  ATH_MSG_DEBUG(" Number of LUCID ROB fragments: " << nLucidFragments);
  
  if (!nLucidFragments) return StatusCode::SUCCESS;

  std::vector<const ROBFragment*>::const_iterator rob_it  = listOfRobf.begin();
  std::vector<const ROBFragment*>::const_iterator rob_end = listOfRobf.end();
  
  for(; rob_it != rob_end; ++rob_it) {
    
    uint32_t robid = (*rob_it)->rod_source_id();
    
    ATH_MSG_DEBUG(" ROB Fragment with ID: " << std::hex << robid << std::dec); 
    
    std::vector<uint32_t> data_block;
    
    StatusCode sc = m_rodDecoder.decode(&**rob_it, data_block);
    
    if (sc.isFailure()) ATH_MSG_WARNING(" Conversion from ByteStream to RawData failed ");
    else {              
      
      ATH_MSG_DEBUG(" Number of data_block elements: " << data_block.size());

      LUCID_RawData* lrd = new LUCID_RawData(data_block);

      ATH_MSG_DEBUG(lrd->str());
      
      lrd->decodeLumatMapping();

      m_LUCID_RawDataContainer->push_back(lrd);
    }
  }
  
  LUCID_RawDataContainer::const_iterator LUCID_RawData_itr = m_LUCID_RawDataContainer->begin();
  LUCID_RawDataContainer::const_iterator LUCID_RawData_end = m_LUCID_RawDataContainer->end();
  
  for (; LUCID_RawData_itr != LUCID_RawData_end; LUCID_RawData_itr++)
    ATH_MSG_DEBUG((*LUCID_RawData_itr)->str());
  
  return StatusCode::SUCCESS;
}

StatusCode LUCID_ByteStreamRawDataCnv::finalize() { 
  
  ATH_MSG_INFO(" LUCID_ByteStreamRawDataCnv::finalize ");
  
  return StatusCode::SUCCESS;
}
