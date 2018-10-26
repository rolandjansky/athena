/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTEventFlagWriter.h"

#include "SCT_ConditionsTools/ISCT_ByteStreamErrorsTool.h"

/// --------------------------------------------------------------------
/// Constructor

SCTEventFlagWriter::SCTEventFlagWriter(const std::string& name,
                                       ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

/// --------------------------------------------------------------------
/// Initialize
StatusCode SCTEventFlagWriter::initialize() 
{
  ATH_CHECK(m_bsErrTool.retrieve());
  ATH_CHECK(m_xAODEvtInfoKey.initialize());
  
  return StatusCode::SUCCESS;
}

/// --------------------------------------------------------------------
/// Execute
StatusCode SCTEventFlagWriter::execute()
{
  long unsigned int nLVL1IDErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::LVL1IDError)->size()};
  long unsigned int nROBFragmentErrors{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::ROBFragmentError)->size()};

  if ((nLVL1IDErrors > 500) or (nROBFragmentErrors > 1000)) {
    //// retrieve EventInfo.  
    /// First the xAOD one
    bool setOK_xAOD{false};
    SG::ReadHandle<xAOD::EventInfo> xAODEvtInfo{m_xAODEvtInfoKey};
    if (xAODEvtInfo.isValid()) {
      setOK_xAOD = xAODEvtInfo->updateErrorState(xAOD::EventInfo::SCT, xAOD::EventInfo::Error);
    } 
    if (not setOK_xAOD) {
      ATH_MSG_ERROR("Failed to retrieve EventInfo containers or to update error state");
      return StatusCode::RECOVERABLE;
    }
  } /// 500 LVL1ID errors or 1000 ROBFragment errors

  return StatusCode::SUCCESS;
}
