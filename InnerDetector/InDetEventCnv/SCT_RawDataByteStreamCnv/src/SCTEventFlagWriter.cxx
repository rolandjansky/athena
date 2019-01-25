/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTEventFlagWriter.h"

#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"

// Constructor

SCTEventFlagWriter::SCTEventFlagWriter(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

// Initialize

StatusCode SCTEventFlagWriter::initialize() 
{
  ATH_CHECK(m_bsErrTool.retrieve());
  ATH_CHECK(m_xAODEvtInfoKey.initialize());
  
  return StatusCode::SUCCESS;
}

// Execute

StatusCode SCTEventFlagWriter::execute(const EventContext& ctx) const
{
  long unsigned int nLVL1IDErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::LVL1IDError, ctx)->size()};
  long unsigned int nROBFragmentErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::ROBFragmentError, ctx)->size()};

  if ((nLVL1IDErrors > 500) or (nROBFragmentErrors > 1000)) { // Check if number of errors exceed threshold
    bool setOK_xAOD{false};
    SG::ReadHandle<xAOD::EventInfo> xAODEvtInfo{m_xAODEvtInfoKey, ctx}; // Retrive xAOD EventInfo
    if (xAODEvtInfo.isValid()) { // Retriving xAOD EventInfo successful
      setOK_xAOD = xAODEvtInfo->updateErrorState(xAOD::EventInfo::SCT, xAOD::EventInfo::Error);
    } 
    if (not setOK_xAOD) {
      ATH_MSG_ERROR("Failed to retrieve EventInfo containers or to update error state");
      return StatusCode::RECOVERABLE;
    }
  }

  return StatusCode::SUCCESS;
}
