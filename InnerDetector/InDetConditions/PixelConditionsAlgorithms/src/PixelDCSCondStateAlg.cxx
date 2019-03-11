/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSCondStateAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>

PixelDCSCondStateAlg::PixelDCSCondStateAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator),
  m_condSvc("CondSvc", name)
{
}

StatusCode PixelDCSCondStateAlg::initialize() {
  ATH_MSG_DEBUG("PixelDCSCondStateAlg::initialize()");

  ATH_CHECK(m_condSvc.retrieve());

  ATH_CHECK(m_readKeyState.initialize());
  ATH_CHECK(m_readKeyStatus.initialize());
  ATH_CHECK(m_writeKeyState.initialize());
  ATH_CHECK(m_writeKeyStatus.initialize());
  if (m_condSvc->regHandle(this, m_writeKeyState).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyState.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  if (m_condSvc->regHandle(this, m_writeKeyStatus).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyStatus.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondStateAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelDCSCondStateAlg::execute()");

  //===========
  // FSM_STATE
  //===========
  SG::WriteCondHandle<PixelModuleData> writeHandleState(m_writeKeyState, ctx);

  if (writeHandleState.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandleState.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
  }
  else {
    SG::ReadCondHandle<CondAttrListCollection> readHandleState(m_readKeyState, ctx);
    const CondAttrListCollection* readCdoState(*readHandleState); 
    if (readCdoState==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object (state)");
      return StatusCode::FAILURE;
    }
    // Get the validitiy range (state)
    EventIDRange rangeState;
    if (not readHandleState.range(rangeState)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleState.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleState.fullKey() << " readCdo->size()= " << readCdoState->size());
    ATH_MSG_INFO("Range of state input is " << rangeState);

    // Construct the output Cond Object and fill it in
    std::unique_ptr<PixelModuleData> writeCdoState(std::make_unique<PixelModuleData>());

    // Read state info
    std::string paramState = "FSM_state";
    for (CondAttrListCollection::const_iterator attrListState=readCdoState->begin(); attrListState!=readCdoState->end(); ++attrListState) {
      const CondAttrListCollection::ChanNum &channelNumber = attrListState->first;
      const CondAttrListCollection::AttributeList &payload = attrListState->second;
      if (payload.exists(paramState.c_str()) and not payload[paramState.c_str()].isNull()) {
        std::string val = payload[paramState.c_str()].data<std::string>();
        if      (val=="READY")      { writeCdoState->setModuleStatus(channelNumber,PixelModuleData::DCSModuleState::READY); }
        else if (val=="ON")         { writeCdoState->setModuleStatus(channelNumber,PixelModuleData::DCSModuleState::ON); }
        else if (val=="UNKNOWN")    { writeCdoState->setModuleStatus(channelNumber,PixelModuleData::DCSModuleState::UNKNOWN); }
        else if (val=="TRANSITION") { writeCdoState->setModuleStatus(channelNumber,PixelModuleData::DCSModuleState::TRANSITION); }
        else if (val=="UNDEFINED")  { writeCdoState->setModuleStatus(channelNumber,PixelModuleData::DCSModuleState::UNDEFINED); }
      } 
      else {
        ATH_MSG_WARNING(paramState << " does not exist for ChanNum " << channelNumber);
        writeCdoState->setModuleStatus(channelNumber,PixelModuleData::DCSModuleState::NOSTATE);
      }
    }

    if (writeHandleState.record(rangeState, std::move(writeCdoState)).isFailure()) {
      ATH_MSG_FATAL("Could not record PixelModuleData " << writeHandleState.key() << " with EventRange " << rangeState << " into Conditions Store");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new CDO " << writeHandleState.key() << " with range " << rangeState << " into Conditions Store");
  }

  //============
  // FSM_STATUS
  //============
  SG::WriteCondHandle<PixelModuleData> writeHandleStatus(m_writeKeyStatus, ctx);

  if (writeHandleStatus.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandleStatus.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
  }
  else {
    SG::ReadCondHandle<CondAttrListCollection> readHandleStatus(m_readKeyStatus, ctx);
    const CondAttrListCollection* readCdoStatus(*readHandleStatus); 
    if (readCdoStatus==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object (state)");
      return StatusCode::FAILURE;
    }

    // Get the validitiy range (state)
    EventIDRange rangeStatus;
    if (not readHandleStatus.range(rangeStatus)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleStatus.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleStatus.fullKey() << " readCdo->size()= " << readCdoStatus->size());
    ATH_MSG_INFO("Range of state input is " << rangeStatus);

    // Construct the output Cond Object and fill it in
    std::unique_ptr<PixelModuleData> writeCdoStatus(std::make_unique<PixelModuleData>());

    // Read state info
    std::string paramStatus = "FSM_status";
    for (CondAttrListCollection::const_iterator attrListStatus=readCdoStatus->begin(); attrListStatus!=readCdoStatus->end(); ++attrListStatus) {
      const CondAttrListCollection::ChanNum &channelNumber = attrListStatus->first;
      const CondAttrListCollection::AttributeList &payload = attrListStatus->second;
      if (payload.exists(paramStatus.c_str()) and not payload[paramStatus.c_str()].isNull()) {
        std::string val = payload[paramStatus.c_str()].data<std::string>();
        if      (val=="OK")      { writeCdoStatus->setModuleStatus(channelNumber,PixelModuleData::DCSModuleStatus::OK); }
        else if (val=="WARNING") { writeCdoStatus->setModuleStatus(channelNumber,PixelModuleData::DCSModuleStatus::WARNING); }
        else if (val=="ERROR")   { writeCdoStatus->setModuleStatus(channelNumber,PixelModuleData::DCSModuleStatus::ERROR); }
        else if (val=="FATAL")   { writeCdoStatus->setModuleStatus(channelNumber,PixelModuleData::DCSModuleStatus::FATAL); }
      } 
      else {
        ATH_MSG_WARNING(paramStatus << " does not exist for ChanNum " << channelNumber);
        writeCdoStatus->setModuleStatus(channelNumber,PixelModuleData::DCSModuleStatus::NOSTATUS);
      }
    }

    if (writeHandleStatus.record(rangeStatus, std::move(writeCdoStatus)).isFailure()) {
      ATH_MSG_FATAL("Could not record PixelModuleData " << writeHandleStatus.key() << " with EventRange " << rangeStatus << " into Conditions Store");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("recorded new CDO " << writeHandleStatus.key() << " with range " << rangeStatus << " into Conditions Store");
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondStateAlg::finalize() {
  ATH_MSG_DEBUG("PixelDCSCondStateAlg::finalize()");
  return StatusCode::SUCCESS;
}

