/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCTriggerCondSvc/TGCTriggerCondAlg.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"


TGCTriggerCondAlg::TGCTriggerCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_readKey_bw("/TGC/TRIGGER/CW_BW_RUN3"),
  m_writeKey("TGCTriggerLUTs"),
  m_condSvc("CondSvc", name),
  m_bwCWReader(0)
{
  declareProperty("ReadKeyBw", m_readKey_bw);
  declareProperty("WriteKey", m_writeKey);
}

StatusCode TGCTriggerCondAlg::initialize(){

  ATH_MSG_INFO( "initialize " << name() );                

  ATH_CHECK(m_condSvc.retrieve());
  
  ATH_CHECK(m_readKey_bw.initialize());

  ATH_CHECK(m_writeKey.initialize());

  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TGCTriggerCondAlg::execute(){
  
  ATH_MSG_DEBUG( "start execute " << name() ); 
    
  SG::WriteCondHandle<TGCTriggerLUTs> writeHandle{m_writeKey};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  auto writeCdo = std::make_unique<TGCTriggerLUTs>();

  // Big wheel for Run3
  SG::ReadCondHandle<CondAttrListCollection> readHandle_bw(m_readKey_bw);
  const CondAttrListCollection* readCdo_bw(*readHandle_bw);

  
  if (readCdo_bw == 0) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE; 
  } 

  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle_bw.fullKey() << " readCdo->size()= " << readCdo_bw->size());
 
  EventIDRange rangeW_bw;
  if ( !readHandle_bw.range(rangeW_bw) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle_bw.key());
    return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO("Range of input is " << rangeW_bw);

  if(!m_bwCWReader.loadParameters(writeCdo.get(), readCdo_bw)){
    ATH_MSG_ERROR("Could not get MessageSvc");
  }

  // fill maps 
  if(!m_bwCWReader.readLUT(writeCdo.get())){
    ATH_MSG_ERROR("Could not get MessageSvc");
  }


  /*
  // other LUTs will be implemented (NSW,RPCBIS78,HotRoI,WichInner...)
  // write condition object
  EventIDRange rangeIntersection = EventIDRange::intersect(rangeW_bw);
  if(rangeIntersection.start()>rangeIntersection.stop()) {
    ATH_MSG_ERROR("Invalid intersection range: " << rangeIntersection);
    return StatusCode::FAILURE;
  }

  if (writeHandle.record(rangeIntersection, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record TGCTriggerLUTs " << writeHandle.key() 
                  << " with EventRange " << rangeIntersection
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }                  
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeIntersection << " into Conditions Store");
  */



  if (writeHandle.record(rangeW_bw, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record TGCTriggerLUTs " << writeHandle.key() 
                  << " with EventRange " << rangeW_bw
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }        



   
  return StatusCode::SUCCESS;
}


StatusCode TGCTriggerCondAlg::finalize(){
  
  ATH_MSG_INFO( "finalize " << name() );
  return StatusCode::SUCCESS;
}
