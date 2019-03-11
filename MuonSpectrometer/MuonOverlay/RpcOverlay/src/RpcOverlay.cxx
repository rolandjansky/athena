/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007
// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008
// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#include <RpcOverlay/RpcOverlay.h>

#include <StoreGate/ReadHandle.h>
#include <StoreGate/WriteHandle.h>

#include <IDC_OverlayBase/IDC_OverlayHelpers.h>


//================================================================
RpcOverlay::RpcOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  IDC_MuonOverlayBase(name, pSvcLocator)
{
}

//================================================================
StatusCode RpcOverlay::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  ATH_CHECK(m_bkgInputKey.initialize());
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey );
  ATH_CHECK(m_signalInputKey.initialize());
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey );
  ATH_CHECK(m_outputKey.initialize());
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey );

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode RpcOverlay::execute()
{
  ATH_MSG_DEBUG("RpcOverlay::execute() begin");


  SG::ReadHandle<RpcDigitContainer> bkgContainer (m_bkgInputKey);
  if (!bkgContainer.isValid()) {
    ATH_MSG_ERROR("Could not get background RPC container " << bkgContainer.name() << " from store " << bkgContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found background RpcDigitContainer called " << bkgContainer.name() << " in store " << bkgContainer.store());
  ATH_MSG_DEBUG("RPC Background = " << Overlay::debugPrint(bkgContainer.cptr()));
  ATH_MSG_VERBOSE("RPC background has digit_size " << bkgContainer->digit_size());

  SG::ReadHandle<RpcDigitContainer> signalContainer(m_signalInputKey);
  if (!signalContainer.isValid() ) {
    ATH_MSG_ERROR("Could not get signal RPC container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal RpcDigitContainer called " << signalContainer.name() << " in store " << signalContainer.store());
  ATH_MSG_DEBUG("RPC Signal     = " << Overlay::debugPrint(signalContainer.cptr()));
  ATH_MSG_VERBOSE("RPC signal has digit_size " << signalContainer->digit_size());

  SG::WriteHandle<RpcDigitContainer> outputContainer(m_outputKey);
  ATH_CHECK(outputContainer.record(std::make_unique<RpcDigitContainer>(bkgContainer->size())));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output RpcDigitContainer called " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output RpcDigitContainer called " << outputContainer.name() << " in store " << outputContainer.store());

  // Do the actual overlay
  ATH_CHECK(overlayMultiHitContainer(bkgContainer.cptr(), signalContainer.cptr(), outputContainer.ptr()));
  ATH_MSG_DEBUG("RPC Result     = " << Overlay::debugPrint(outputContainer.cptr()));


  ATH_MSG_DEBUG("RpcOverlay::execute() end");

  return StatusCode::SUCCESS;
}
