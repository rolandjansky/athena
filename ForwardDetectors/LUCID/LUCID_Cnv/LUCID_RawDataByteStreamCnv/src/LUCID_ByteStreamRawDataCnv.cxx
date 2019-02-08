/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "LUCID_RawDataByteStreamCnv/LUCID_ByteStreamRawDataCnv.h"
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

LUCID_ByteStreamRawDataCnv::LUCID_ByteStreamRawDataCnv(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm     (name                , pSvcLocator),
  m_robDataProvider("ROBDataProviderSvc", name) {
}

LUCID_ByteStreamRawDataCnv::~LUCID_ByteStreamRawDataCnv(){}

StatusCode LUCID_ByteStreamRawDataCnv::initialize() {
  
  ATH_MSG_INFO("LUCID_ByteStreamRawDataCnv::initialize");

  StatusCode sc = m_robDataProvider.retrieve();
  
  if (sc.isFailure()) ATH_MSG_WARNING(" Could not retrieve ROBDataProviderSvc ");
  else                ATH_MSG_DEBUG  (" Retrieved service ROBDataProviderSvc ");

  ATH_CHECK( m_lucid_RawDataContainerKey.initialize() );
  
  return StatusCode::SUCCESS;
}

StatusCode LUCID_ByteStreamRawDataCnv::execute() {

  const EventContext& ctx = Gaudi::Hive::currentContext();
  ATH_MSG_DEBUG(" LUCID_ByteStreamRawDataCnv::execute ");
  
  std::vector<const ROBFragment*> listOfRobf;
  std::vector<unsigned int> ROBIDs;
  
  ROBIDs.push_back(m_rodDecoder.getSourceID());
  
  m_robDataProvider->getROBData(ROBIDs, listOfRobf);
  
  auto container = std::make_unique<LUCID_RawDataContainer>();
  
  StatusCode sc = fillContainer(listOfRobf, *container);
  
  if (sc.isFailure()) ATH_MSG_WARNING(" fillContainer failed ");
  else                ATH_MSG_DEBUG  (" fillContainer success ");

  ATH_CHECK( SG::makeHandle (m_lucid_RawDataContainerKey, ctx).record (std::move (container)) );
  
  return StatusCode::SUCCESS;
}

StatusCode
LUCID_ByteStreamRawDataCnv::fillContainer(const std::vector<const ROBFragment*>& listOfRobf,
                                          LUCID_RawDataContainer& container) const {
  
  ATH_MSG_DEBUG(" LUCID_ByteStreamRawDataCnv::fillContainer ");
  
  int nLucidFragments = listOfRobf.size();
    
  ATH_MSG_DEBUG(" Number of LUCID ROB fragments: " << nLucidFragments);
  
  if (!nLucidFragments) return StatusCode::SUCCESS;

  for (const ROBFragment* frag : listOfRobf) {
    
    uint32_t robid = frag->rod_source_id();
    
    ATH_MSG_DEBUG(" ROB Fragment with ID: " << std::hex << robid << std::dec); 
    
    std::vector<uint32_t> data_block;
    
    StatusCode sc = m_rodDecoder.decode(frag, data_block, msg());
    
    if (sc.isFailure()) ATH_MSG_WARNING(" Conversion from ByteStream to RawData failed ");
    else {              
      
      ATH_MSG_DEBUG(" Number of data_block elements: " << data_block.size());

      LUCID_RawData* lrd = new LUCID_RawData(data_block);

      ATH_MSG_DEBUG(lrd->str());
      
      lrd->decodeLumatMapping();

      container.push_back(lrd);
    }
  }
  
  for (const LUCID_RawData* data : container)
    ATH_MSG_DEBUG(data->str());
  
  return StatusCode::SUCCESS;
}

StatusCode LUCID_ByteStreamRawDataCnv::finalize() { 
  
  ATH_MSG_INFO(" LUCID_ByteStreamRawDataCnv::finalize ");
  
  return StatusCode::SUCCESS;
}
