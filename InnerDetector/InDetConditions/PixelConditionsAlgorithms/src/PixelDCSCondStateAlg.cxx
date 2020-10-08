/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSCondStateAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>

PixelDCSCondStateAlg::PixelDCSCondStateAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelDCSCondStateAlg::initialize() {
  ATH_MSG_DEBUG("PixelDCSCondStateAlg::initialize()");

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));
  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_readKeyState.initialize(SG::AllowEmpty));
  ATH_CHECK(m_writeKeyState.initialize());
  if (m_condSvc->regHandle(this, m_writeKeyState).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyState.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondStateAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelDCSCondStateAlg::execute()");

  SG::WriteCondHandle<PixelDCSStateData> writeHandleState(m_writeKeyState, ctx);
  if (writeHandleState.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandleState.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelDCSStateData> writeCdoState(std::make_unique<PixelDCSStateData>());
  
  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0,                       0,                       EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop {EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};

  EventIDRange rangeW{start, stop};

  if (!m_readKeyState.empty()) {
    SG::ReadCondHandle<CondAttrListCollection> readHandle(m_readKeyState, ctx);
    const CondAttrListCollection* readCdoState(*readHandle); 
    if (readCdoState==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object (state)");
      return StatusCode::FAILURE;
    }
    // Get the validitiy range
    if (not readHandle.range(rangeW)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdoState->size());
    ATH_MSG_INFO("Range of state input is " << rangeW);

    // Read state info
    std::string paramState = "FSM_state";
    for (CondAttrListCollection::const_iterator attrListState=readCdoState->begin(); attrListState!=readCdoState->end(); ++attrListState) {
      const CondAttrListCollection::ChanNum &channelNumber = attrListState->first;
      const CondAttrListCollection::AttributeList &payload = attrListState->second;
      if (payload.exists(paramState.c_str()) and not payload[paramState.c_str()].isNull()) {
        std::string val = payload[paramState.c_str()].data<std::string>();
        if      (val=="READY")      { writeCdoState->setModuleStatus(channelNumber,PixelDCSStateData::DCSModuleState::READY); }
        else if (val=="ON")         { writeCdoState->setModuleStatus(channelNumber,PixelDCSStateData::DCSModuleState::ON); }
        else if (val=="UNKNOWN")    { writeCdoState->setModuleStatus(channelNumber,PixelDCSStateData::DCSModuleState::UNKNOWN); }
        else if (val=="TRANSITION") { writeCdoState->setModuleStatus(channelNumber,PixelDCSStateData::DCSModuleState::TRANSITION); }
        else if (val=="UNDEFINED")  { writeCdoState->setModuleStatus(channelNumber,PixelDCSStateData::DCSModuleState::UNDEFINED); }
      } 
      else {
        ATH_MSG_WARNING(paramState << " does not exist for ChanNum " << channelNumber);
        writeCdoState->setModuleStatus(channelNumber,PixelDCSStateData::DCSModuleState::NOSTATE);
      }
    }
  }
  else {  // Set READY for enough large numbers
    for (int i=0; i<(int)m_pixelID->wafer_hash_max(); i++) { writeCdoState->setModuleStatus(i,PixelDCSStateData::DCSModuleState::READY); }
  }

  if (writeHandleState.record(rangeW, std::move(writeCdoState)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelDCSStateData " << writeHandleState.key() << " with EventRange " << writeHandleState.getRange() << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandleState.key() << " with range " << writeHandleState.getRange() << " into Conditions Store");

  return StatusCode::SUCCESS;
}

