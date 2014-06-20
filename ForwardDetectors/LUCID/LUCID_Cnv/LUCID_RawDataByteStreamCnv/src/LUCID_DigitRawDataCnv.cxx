/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "LUCID_RawDataByteStreamCnv/LUCID_DigitRawDataCnv.h"

LUCID_DigitRawDataCnv::LUCID_DigitRawDataCnv(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm     (name                , pSvcLocator),
  m_storeGate      ("StoreGateSvc"      , name) 
{
  declareProperty("lucid_RawDataContainerKey", m_lucid_RawDataContainerKey = "Lucid_RawData");
  declareProperty("lucid_DigitContainerKey",   m_digitContainerKey         = "Lucid_Digits");

  m_digitContainer = 0;
  m_LUCID_RawDataContainer = 0;
}

LUCID_DigitRawDataCnv::~LUCID_DigitRawDataCnv(){}

StatusCode LUCID_DigitRawDataCnv::initialize() {
  
  ATH_MSG_INFO("LUCID_DigitRawDataCnv::initialize");

  return StatusCode::SUCCESS;
}

StatusCode LUCID_DigitRawDataCnv::execute() {
  
  ATH_MSG_DEBUG(" LUCID_DigitRawDataCnv::execute ");
  
  StatusCode sc = evtStore()->retrieve(m_digitContainer, m_digitContainerKey);
  
  if (sc.isFailure()) {
    
    ATH_MSG_WARNING("BAD DATA!!! Input data does not include " << m_digitContainerKey);
    
    return StatusCode::SUCCESS;
  }
  
  int nLucidDigits = m_digitContainer->size();
  
  ATH_MSG_DEBUG(" Number of LUCID Digits: " << nLucidDigits);
  
  if (!nLucidDigits) return StatusCode::SUCCESS;

  LUCID_DigitContainer::const_iterator it_cont     = m_digitContainer->begin(); 
  LUCID_DigitContainer::const_iterator it_cont_end = m_digitContainer->end();

  for( ; it_cont != it_cont_end; ++it_cont)
    m_rodEncoder.addDigit((*it_cont));
  
  std::vector<uint32_t> data_block;
  
  m_rodEncoder.encode(data_block);

  data_block.push_back(0); // add status word
  
  m_LUCID_RawDataContainer = new LUCID_RawDataContainer();

  m_LUCID_RawDataContainer->push_back(new LUCID_RawData(data_block));
  
  sc = m_storeGate->record(m_LUCID_RawDataContainer, m_lucid_RawDataContainerKey);
  
  if (sc.isFailure()) ATH_MSG_WARNING(" Could not record LUCID_RawDataContainer in StoreGate ");
  else                ATH_MSG_DEBUG  (" LUCID_RawDataContainer is recorded in StoreGate ");
  
  return StatusCode::SUCCESS;
 }
 
 StatusCode LUCID_DigitRawDataCnv::finalize() { 
   
   ATH_MSG_INFO(" LUCID_DigitRawDataCnv::finalize ");
   
   return StatusCode::SUCCESS;
 }
