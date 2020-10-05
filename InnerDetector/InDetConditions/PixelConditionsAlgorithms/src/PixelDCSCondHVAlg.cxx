/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSCondHVAlg.h"
#include "Identifier/IdentifierHash.h"
#include <memory>

PixelDCSCondHVAlg::PixelDCSCondHVAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelDCSCondHVAlg::initialize() {
  ATH_MSG_DEBUG("PixelDCSCondHVAlg::initialize()");

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_moduleDataKey.initialize());
  ATH_CHECK(m_readKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondHVAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelDCSCondHVAlg::execute()");

  SG::WriteCondHandle<PixelDCSHVData> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }
  
  SG::ReadCondHandle<PixelModuleData> modDataHdl(m_moduleDataKey,ctx);
  const PixelModuleData* modData=(*modDataHdl);
  ATH_MSG_INFO("Range of input PixelModule data is " << modDataHdl.getRange()); 

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelDCSHVData> writeCdo(std::make_unique<PixelDCSHVData>());

  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0,                       0,                       EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop {EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};

  EventIDRange rangeW{start, stop};

  if (!m_readKey.empty()) {
    SG::ReadCondHandle<CondAttrListCollection> readHandle(m_readKey, ctx);
    const CondAttrListCollection* readCdo = *readHandle; 
    if (readCdo==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object");
      return StatusCode::FAILURE;
    }
    // Get the validitiy range
    if (not readHandle.range(rangeW)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of DCS input is " << rangeW);

    // Read HV info
    std::string param("HV");
    for (CondAttrListCollection::const_iterator attrList=readCdo->begin(); attrList!=readCdo->end(); ++attrList) {
      const CondAttrListCollection::ChanNum &channelNumber = attrList->first;
      const CondAttrListCollection::AttributeList &payload = attrList->second;
      if (payload.exists(param) and not payload[param].isNull()) {
        float val = payload[param].data<float>();
        if (val>1000.0 || val<-1000.0) {
          writeCdo -> setBiasVoltage((int)channelNumber, modData->getDefaultBiasVoltage());
        }
        else {
          writeCdo -> setBiasVoltage((int)channelNumber, val);
        }
      } 
      else {
        ATH_MSG_WARNING(param << " does not exist for ChanNum " << channelNumber);
        writeCdo -> setBiasVoltage((int)channelNumber, modData->getDefaultBiasVoltage());
      }
    }
  }
  else {
    for (int i=0; i<(int)m_pixelID->wafer_hash_max(); i++) {
      writeCdo -> setBiasVoltage(i, modData->getDefaultBiasVoltage());
    }
  }

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelDCSHVData " << writeHandle.key() << " with EventRange " 
		  << writeHandle.getRange() << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << writeHandle.getRange() 
	       << " into Conditions Store");

  return StatusCode::SUCCESS;
}

