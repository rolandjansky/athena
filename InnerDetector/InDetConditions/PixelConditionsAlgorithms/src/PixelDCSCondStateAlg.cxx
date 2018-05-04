/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSCondStateAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"

#include <memory>

PixelDCSCondStateAlg::PixelDCSCondStateAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthAlgorithm(name, pSvcLocator),
  m_readKeyState("/PIXEL/DCS/FSMSTATUS"),
  m_writeKeyState("PixelDCSStateCondData"),
  m_condSvc("CondSvc", name),
  m_readAllDBFolders(true)
{
  declareProperty("ReadAllDBFolders", m_readAllDBFolders);
  declareProperty("ReadKeyState", m_readKeyState, "Key of input (raw) State conditions folder");
  declareProperty("WriteKeyState", m_writeKeyState, "Key of output (derived) State conditions folder");
}

StatusCode PixelDCSCondStateAlg::initialize() {
  ATH_MSG_INFO("PixelDCSCondStateAlg::initialize()");

  ATH_CHECK(m_condSvc.retrieve());

  if (m_readAllDBFolders) {
    ATH_CHECK(m_readKeyState.initialize());
    ATH_CHECK(m_writeKeyState.initialize());
    if (m_condSvc->regHandle(this, m_writeKeyState).isFailure()) {
      ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKeyState.fullKey() << " with CondSvc");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondStateAlg::execute() {
  ATH_MSG_INFO("PixelDCSCondStateAlg::execute()");

  if (not m_readAllDBFolders) { return StatusCode::SUCCESS; }

  // Write Cond Handle (state)
  SG::WriteCondHandle<PixelDCSCondData> writeHandle{m_writeKeyState};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Read Cond Handle (state)
  SG::ReadCondHandle<CondAttrListCollection> readHandleState(m_readKeyState);
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
  std::unique_ptr<PixelDCSCondData> writeCdoState(std::make_unique<PixelDCSCondData>());

  // Read state info
  std::string paramState("FSM_status");
  CondAttrListCollection::const_iterator attrListState(readCdoState->begin());
  CondAttrListCollection::const_iterator endState(readCdoState->end());
  // CondAttrListCollection doesn't support C++11 type loops, no generic 'begin'
  for (; attrListState!=endState; ++attrListState) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum channelNumber(attrListState->first);
    CondAttrListCollection::AttributeList payload(attrListState->second);
    if (payload.exists(paramState.c_str()) and not payload[paramState.c_str()].isNull()) {
//      unsigned int val(payload[paramState.c_str()].data<unsigned int>());
      std::string val(payload[paramState.c_str()].data<std::string>());

// STSTST      std::cout << "STSTST FSMState " << val << std::endl;

//      writeCdoState->fill(channelNumber, paramState);
    } 
    else {
      ATH_MSG_WARNING(paramState << " does not exist for ChanNum " << channelNumber);
    }
  }

  // Record the output cond object
  if (writeHandle.record(rangeState, std::move(writeCdoState)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelDCSCondData " << writeHandle.key() << " with EventRange " << rangeState << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeState << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondStateAlg::finalize() {
  ATH_MSG_INFO("PixelDCSCondStateAlg::finalize()");
  return StatusCode::SUCCESS;
}

