/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_DCSConditionsHVCondAlg.h"

#include "Identifier/IdentifierHash.h"
#include "SCT_Cabling/SCT_OnlineId.h"

#include "GaudiKernel/EventIDRange.h"

SCT_DCSConditionsHVCondAlg::SCT_DCSConditionsHVCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_readKey{"/SCT/DCS/HV"}
  , m_writeKey{"SCT_DCSHVCondData"}
  , m_condSvc{"CondSvc", name}
  , m_returnHVTemp{true}
{
  declareProperty("ReturnHVTemp", m_returnHVTemp);
  
  declareProperty("ReadKey", m_readKey, "Key of input (raw) HV conditions folder");
  declareProperty("WriteKey", m_writeKey, "Key of output (derived) HV conditions folder");
}

SCT_DCSConditionsHVCondAlg::~SCT_DCSConditionsHVCondAlg() {
}

StatusCode SCT_DCSConditionsHVCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());

  if (m_returnHVTemp) {
    // Read Cond Handle
    ATH_CHECK(m_readKey.initialize());
    // Write Cond Handle
    ATH_CHECK(m_writeKey.initialize());
    if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
      ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsHVCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  if (not m_returnHVTemp) {
    return StatusCode::SUCCESS;
  }

  // Write Cond Handle
  SG::WriteCondHandle<SCT_DCSFloatCondData> writeHandle{m_writeKey};
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
  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_INFO("Range of input is " << rangeW);
  
  // Construct the output Cond Object and fill it in
  SCT_DCSFloatCondData* writeCdo{new SCT_DCSFloatCondData()};

  // Read temperature info
  std::string param{"HVCHVOLT_RECV"};
  CondAttrListCollection::const_iterator attrList{readCdo->begin()};
  CondAttrListCollection::const_iterator end{readCdo->end()};
  // CondAttrListCollection doesn't support C++11 type loops, no generic 'begin'
  for (; attrList!=end; ++attrList) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum channelNumber{attrList->first};
    CondAttrListCollection::AttributeList payload{attrList->second};
    if (payload.exists(param) and not payload[param].isNull()) {
      float val{payload[param].data<float>()};
      writeCdo->setValue(channelNumber, val);
    } else {
      ATH_MSG_WARNING(param << " does not exist for ChanNum " << channelNumber);
    }
  }

  // Record the output cond object
  if (writeHandle.record(rangeW, writeCdo).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_DCSFloatCondData " << writeHandle.key() 
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    delete writeCdo;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_DCSConditionsHVCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
