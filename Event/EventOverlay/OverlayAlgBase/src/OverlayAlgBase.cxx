/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko, 2006-2009

#include "OverlayAlgBase/OverlayAlgBase.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "StoreGate/StoreGateSvc.h"

//================================================================
OverlayAlgBase::OverlayAlgBase(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_storeGateData("StoreGateSvc/OriginalEvent_SG", name),
  m_storeGateMC("StoreGateSvc/BkgEvent_0_SG", name),
  m_storeGateOutput("StoreGateSvc/StoreGateSvc", name)
{
  declareProperty("DataStore", m_storeGateData, "help");
  declareProperty("MCStore", m_storeGateMC, "help");
  declareProperty("OutputStore", m_storeGateOutput, "help"); //FIXME this should be dropped in favour of the evtStore method of AthAlgorithm
}

//================================================================
StatusCode OverlayAlgBase::initialize()
{
  ATH_MSG_DEBUG("OverlayAlgBase::initialize()");

  if (m_storeGateData.retrieve().isFailure()) {
    ATH_MSG_FATAL("OverlayAlgBase::initialize): StoreGate[data] service not found !");
    return StatusCode::FAILURE;
  }

  if (m_storeGateMC.retrieve().isFailure()) {
    ATH_MSG_FATAL("OverlayAlgBase::initialize): StoreGate[MC] service not found !");
    return StatusCode::FAILURE;
  }

  if (m_storeGateOutput.retrieve().isFailure()) {
    ATH_MSG_FATAL("OverlayAlgBase::initialize): StoreGate[output] service not found !");
    return StatusCode::FAILURE;
  }

  // Call initialization() of a derived class.
  return overlayInitialize();
}

//================================================================
StatusCode OverlayAlgBase::execute() {
  ATH_MSG_DEBUG("OverlayAlgBase::execute() begin. Calling derived class execute()");
  StatusCode ret = overlayExecute();
  ATH_MSG_DEBUG("OverlayAlgBase::execute() end");
  return ret;
}

//================================================================
StatusCode OverlayAlgBase::finalize() {
  return overlayFinalize();
}

//================================================================
//EOF
