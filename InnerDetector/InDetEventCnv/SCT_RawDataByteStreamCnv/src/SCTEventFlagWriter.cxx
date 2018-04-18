/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTEventFlagWriter.h"

/// --------------------------------------------------------------------
/// Constructor

SCTEventFlagWriter::SCTEventFlagWriter(const std::string& name,
                                       ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
  declareProperty("xAODEventInfoKey", m_xevtInfoKey=std::string{"EventInfo"});
  declareProperty("EventInfoKey", m_evtInfoKey=std::string{"ByteStreamEventInfo"});
}

/// --------------------------------------------------------------------
/// Initialize
StatusCode SCTEventFlagWriter::initialize() {
  ATH_CHECK(m_bsErrTool.retrieve());
  ATH_CHECK(m_xevtInfoKey.initialize());
  ATH_CHECK(m_evtInfoKey.initialize());
  
  return StatusCode::SUCCESS;
}

/// --------------------------------------------------------------------
/// Execute
StatusCode SCTEventFlagWriter::execute()
{
  long unsigned int nLVL1ID{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::LVL1IDError)->size()};
  long unsigned int nROBFragment{m_bsErrTool->getErrorSet(SCT_ByteStreamErrors::ROBFragmentError)->size()};

  if ((nLVL1ID > 500) or (nROBFragment > 1000)) {
    //// retrieve EventInfo.  
    /// First the xAOD one
    bool setOK_xAOD{false};
    SG::ReadHandle<xAOD::EventInfo> xevtInfo_const{m_xevtInfoKey};
    if (xevtInfo_const.isValid()) {
      xAOD::EventInfo* xevtInfo{const_cast<xAOD::EventInfo*>(&*xevtInfo_const)};
      setOK_xAOD = xevtInfo->setErrorState(xAOD::EventInfo::SCT, xAOD::EventInfo::Error);
    } 

    /// Second the old-style one
    bool setOK_old{false};
    SG::ReadHandle<EventInfo> evtInfo_const{m_evtInfoKey};
    if (evtInfo_const.isValid()) {
      EventInfo* evtInfo{const_cast<EventInfo*>(&*evtInfo_const)};
      setOK_old = evtInfo->setErrorState(EventInfo::SCT, EventInfo::Error);
    }

    if ((not setOK_xAOD) and (not setOK_old)) {
      ATH_MSG_ERROR("Failed to retrieve EventInfo containers or to set error states");
      return StatusCode::RECOVERABLE;
    }
  } /// 500 LVL1ID errors or 1000 ROBFragment errors

  return StatusCode::SUCCESS;
}
