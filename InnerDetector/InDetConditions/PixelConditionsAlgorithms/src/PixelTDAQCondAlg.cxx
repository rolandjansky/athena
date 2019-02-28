/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelTDAQCondAlg.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>
#include <sstream>

PixelTDAQCondAlg::PixelTDAQCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator),
  m_pixelID(nullptr),
  m_condSvc("CondSvc", name)
{
}

StatusCode PixelTDAQCondAlg::initialize() {
  ATH_MSG_INFO("PixelTDAQCondAlg::initialize()");

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

  ATH_CHECK(m_condSvc.retrieve());

  ATH_CHECK(m_readKey.initialize());
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode PixelTDAQCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_INFO("PixelTDAQCondAlg::execute()");

  SG::WriteCondHandle<PixelModuleData> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  SG::ReadCondHandle<CondAttrListCollection> readHandle(m_readKey, ctx);
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
  std::unique_ptr<PixelModuleData> writeCdo(std::make_unique<PixelModuleData>());

  // Read dead map info
  std::vector<int> checkActive;
  for (CondAttrListCollection::const_iterator attrList=readCdo->begin(); attrList!=readCdo->end(); ++attrList) {
    CondAttrListCollection::ChanNum channelNumber = attrList->first;
    CondAttrListCollection::AttributeList payload = attrList->second;
    checkActive.push_back((int)channelNumber-1);
  }
  
  for (int i=0; i<(int)m_pixelID->wafer_hash_max(); i++) {
    auto itr = std::find(checkActive.begin(),checkActive.end(),i);
    if ((size_t)std::distance(checkActive.begin(),itr)==checkActive.size()) { writeCdo->setModuleStatus(i,1); }
  }

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelModuleData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode PixelTDAQCondAlg::finalize() {
  ATH_MSG_INFO("PixelTDAQCondAlg::finalize()");
  return StatusCode::SUCCESS;
}

