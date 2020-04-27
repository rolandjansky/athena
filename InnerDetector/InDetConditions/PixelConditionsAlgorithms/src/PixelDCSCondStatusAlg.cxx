/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSCondStatusAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>

PixelDCSCondStatusAlg::PixelDCSCondStatusAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelDCSCondStatusAlg::initialize() {
  ATH_MSG_DEBUG("PixelDCSCondStatusAlg::initialize()");

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));
  ATH_CHECK(m_condSvc.retrieve());

  ATH_CHECK(m_moduleDataKey.initialize());
  ATH_CHECK(m_readKeyStatus.initialize(!m_readKeyStatus.empty()));
  ATH_CHECK(m_writeKeyStatus.initialize());
  if (m_condSvc->regHandle(this, m_writeKeyStatus).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyStatus.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondStatusAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelDCSCondStatusAlg::execute()");

  SG::WriteCondHandle<PixelDCSStatusData> writeHandleStatus(m_writeKeyStatus, ctx);
  if (writeHandleStatus.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandleStatus.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelDCSStatusData> writeCdoStatus(std::make_unique<PixelDCSStatusData>());

 
  SG::ReadCondHandle<PixelModuleData> modDataHdl(m_moduleDataKey,ctx);
  const PixelModuleData* modData=(*modDataHdl);
  writeHandleStatus.addDependency(modDataHdl);
  ATH_MSG_INFO("Range of input PixelModule data is " << modDataHdl.getRange()); 
  

  if (modData->getUseDCSStatusConditions()) {
    SG::ReadCondHandle<CondAttrListCollection> readHandleStatus(m_readKeyStatus, ctx);
    const CondAttrListCollection* readCdoStatus(*readHandleStatus); 
    if (readCdoStatus==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object (state)");
      return StatusCode::FAILURE;
    }
    writeHandleStatus.addDependency(readHandleStatus);
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleStatus.fullKey() << " readCdo->size()= " << readCdoStatus->size());
    ATH_MSG_INFO("Range of state input is " << readHandleStatus.getRange());

    // Read state info
    const std::string paramStatus = "FSM_status";
    for (CondAttrListCollection::const_iterator attrListStatus=readCdoStatus->begin(); attrListStatus!=readCdoStatus->end(); ++attrListStatus) {
      const CondAttrListCollection::ChanNum &channelNumber = attrListStatus->first;
      const CondAttrListCollection::AttributeList &payload = attrListStatus->second;
      if (payload.exists(paramStatus.c_str()) and not payload[paramStatus.c_str()].isNull()) {
        const std::string& val = payload[paramStatus.c_str()].data<std::string>();
        if      (val=="OK")      { writeCdoStatus->setModuleStatus(channelNumber,PixelDCSStatusData::DCSModuleStatus::OK); }
        else if (val=="WARNING") { writeCdoStatus->setModuleStatus(channelNumber,PixelDCSStatusData::DCSModuleStatus::WARNING); }
        else if (val=="ERROR")   { writeCdoStatus->setModuleStatus(channelNumber,PixelDCSStatusData::DCSModuleStatus::ERROR); }
        else if (val=="FATAL")   { writeCdoStatus->setModuleStatus(channelNumber,PixelDCSStatusData::DCSModuleStatus::FATAL); }
      } 
      else {
        ATH_MSG_WARNING(paramStatus << " does not exist for ChanNum " << channelNumber);
        writeCdoStatus->setModuleStatus(channelNumber,PixelDCSStatusData::DCSModuleStatus::NOSTATUS);
      }
    }
  }
  else {
    for (int i=0; i<(int)m_pixelID->wafer_hash_max(); i++) { 
      writeCdoStatus->setModuleStatus(i,PixelDCSStatusData::DCSModuleStatus::OK); 
    }
  }

  if (writeHandleStatus.record(std::move(writeCdoStatus)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelDCSStatusData " << writeHandleStatus.key() << " with EventRange " 
		  << writeHandleStatus.getRange() << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandleStatus.key() << " with range " 
	       << writeHandleStatus.getRange() << " into Conditions Store");

  return StatusCode::SUCCESS;
}

