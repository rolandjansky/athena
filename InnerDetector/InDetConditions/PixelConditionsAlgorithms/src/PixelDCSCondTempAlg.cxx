/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSCondTempAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"

#include <memory>

PixelDCSCondTempAlg::PixelDCSCondTempAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthAlgorithm(name, pSvcLocator),
  m_condSvc("CondSvc", name)
{
}

StatusCode PixelDCSCondTempAlg::initialize() {
  ATH_MSG_DEBUG("PixelDCSCondTempAlg::initialize()");

  ATH_CHECK(m_condSvc.retrieve());

  ATH_CHECK(m_readKey.initialize());
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondTempAlg::execute() {
  ATH_MSG_DEBUG("PixelDCSCondTempAlg::execute()");

  SG::WriteCondHandle<PixelDCSConditionsData> writeHandle(m_writeKey);
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle(m_readKey);
  const CondAttrListCollection* readCdo(*readHandle); 
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
  std::unique_ptr<PixelDCSConditionsData> writeCdo(std::make_unique<PixelDCSConditionsData>());

  // Read temperature info
  std::string param{"temperature"};
  for (CondAttrListCollection::const_iterator attrList=readCdo->begin(); attrList!=readCdo->end(); ++attrList) {
    const CondAttrListCollection::ChanNum &channelNumber{attrList->first};
    const CondAttrListCollection::AttributeList &payload{attrList->second};
    if (payload.exists(param) and not payload[param].isNull()) {
      float val = payload[param].data<float>();
      writeCdo->setValue(channelNumber, val);
    } 
    else {
      ATH_MSG_WARNING(param << " does not exist for ChanNum " << channelNumber);
      writeCdo->setValue(channelNumber, -88.0);
    }
  }

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelDCSConditionsData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondTempAlg::finalize() {
  ATH_MSG_DEBUG("PixelDCSCondTempAlg::finalize()");
  return StatusCode::SUCCESS;
}

