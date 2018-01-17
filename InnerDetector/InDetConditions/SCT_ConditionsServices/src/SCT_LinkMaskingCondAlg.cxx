/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_LinkMaskingCondAlg.h"

#include "GaudiKernel/EventIDRange.h"

SCT_LinkMaskingCondAlg::SCT_LinkMaskingCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_readKey{"/purple/pants"}
  // This folder can be created by SCT_ConditionsServices/python/createLinkMaskingSQLiteFile.py
  , m_writeKey{"SCT_LinkMaskingCondData"}
  , m_condSvc{"CondSvc", name}
{
  declareProperty("ReadKey", m_readKey, "Key of input (raw) bad wafer conditions folder");
  declareProperty("WriteKey", m_writeKey, "Key of output (derived) bad wafer conditions folder");
}

SCT_LinkMaskingCondAlg::~SCT_LinkMaskingCondAlg() {
}

StatusCode SCT_LinkMaskingCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());

  // Read Cond Handle
  ATH_CHECK(m_readKey.initialize());
  // Write Cond Handles
  ATH_CHECK(m_writeKey.initialize());
  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_LinkMaskingCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<SCT_ModuleVetoCondData> writeHandle{m_writeKey};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS; 
  }

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey};
  const CondAttrListCollection* readCdo{*readHandle}; 
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  // Get the validitiy range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of AthenaAttributeList " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_INFO("Range of input is " << rangeW);
  
  // Construct the output Cond Object and fill it in
  SCT_ModuleVetoCondData* writeCdo{new SCT_ModuleVetoCondData()};

  // Read bad wafer info
  CondAttrListCollection::const_iterator linkItr{readCdo->begin()};
  CondAttrListCollection::const_iterator linkEnd{readCdo->end()};
  for (;linkItr != linkEnd; ++linkItr) {
    //A CondAttrListCollection is a map of ChanNum and AttributeList
    Identifier waferId{(*linkItr).first};
    CondAttrListCollection::AttributeList payload{(*linkItr).second};
    bool lastProbedState{payload[0].data<bool>()};
    if (not lastProbedState) writeCdo->setBadWaferId(waferId);
    ATH_MSG_INFO("LINK " << waferId << " (" << waferId.get_identifier32().get_compact() << " in 32 bit): " << lastProbedState);
  }
  if (writeCdo->size()!=0) writeCdo->setFilled();

  // Record the output cond object
  if (writeHandle.record(rangeW, writeCdo).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_ModuleVetoCondData " << writeHandle.key() 
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_LinkMaskingCondAlg::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
