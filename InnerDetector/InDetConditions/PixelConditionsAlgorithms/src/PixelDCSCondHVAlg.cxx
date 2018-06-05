/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSCondHVAlg.h"

#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>

PixelDCSCondHVAlg::PixelDCSCondHVAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthAlgorithm(name, pSvcLocator),
  m_readKey("/PIXEL/DCS/HV"),
  m_writeKey("PixelDCSHVCondData"),
  m_condSvc("CondSvc", name),
  m_returnHVTemp(true)
{
  declareProperty("ReturnHVTemp", m_returnHVTemp);
  declareProperty("ReadKey", m_readKey, "Key of input (raw) HV conditions folder");
  declareProperty("WriteKey", m_writeKey, "Key of output (derived) HV conditions folder");
}

StatusCode PixelDCSCondHVAlg::initialize() {
  ATH_MSG_INFO("PixelDCSCondHVAlg::initialize()");

  ATH_CHECK(m_condSvc.retrieve());

  if (m_returnHVTemp) {
    ATH_CHECK(m_readKey.initialize());
    ATH_CHECK(m_writeKey.initialize());
    if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
      ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondHVAlg::execute() {
  ATH_MSG_INFO("PixelDCSCondHVAlg::execute()");

  if (not m_returnHVTemp) { return StatusCode::SUCCESS; }

  SG::WriteCondHandle<PixelDCSCondData> writeHandle(m_writeKey);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  SG::ReadCondHandle<CondAttrListCollection> readHandle(m_readKey);
  const CondAttrListCollection* readCdo = *readHandle; 
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
  std::unique_ptr<PixelDCSCondData> writeCdo(std::make_unique<PixelDCSCondData>());

  // Read HV info
  std::string param("HV");
  for (CondAttrListCollection::const_iterator attrList=readCdo->begin(); attrList!=readCdo->end(); ++attrList) {
    CondAttrListCollection::ChanNum channelNumber = attrList->first;
    CondAttrListCollection::AttributeList payload = attrList->second;
    if (payload.exists(param) and not payload[param].isNull()) {
      float val(payload[param].data<float>());

// STSTST      std::cout << "STSTST HV " << rangeW << " " << channelNumber << " " << val << std::endl;

      writeCdo->setValue(channelNumber, val);
    } 
    else {
      ATH_MSG_WARNING(param << " does not exist for ChanNum " << channelNumber);
    }
  }

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelDCSCondData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondHVAlg::finalize() {
  ATH_MSG_INFO("PixelDCSCondHVAlg::finalize()");
  return StatusCode::SUCCESS;
}

