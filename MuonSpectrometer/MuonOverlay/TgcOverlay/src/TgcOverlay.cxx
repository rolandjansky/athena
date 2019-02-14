/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008

// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#include "TgcOverlay/TgcOverlay.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonDigitContainer/TgcDigitContainer.h"

#include <iostream>
#include <typeinfo>

//================================================================
TgcOverlay::TgcOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  IDC_MultiHitOverlayBase(name, pSvcLocator)
{
}

//================================================================
StatusCode TgcOverlay::overlayInitialize()
{
  ATH_MSG_INFO("TgcOverlay initialized");

  /** access to the TGC Identifier helper */
  ATH_CHECK(detStore()->retrieve(m_tgcHelper, "TGCIDHELPER"));
  ATH_MSG_DEBUG(" Found the TgcIdHelper. ");

  ATH_CHECK(m_mainInputDigitKey.initialize());
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_mainInputDigitKey );
  ATH_CHECK(m_overlayInputDigitKey.initialize());
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_overlayInputDigitKey );
  ATH_CHECK(m_outputDigitKey.initialize());
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputDigitKey );

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode TgcOverlay::overlayFinalize()
{
  ATH_MSG_INFO("TgcOverlay finalized");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode TgcOverlay::overlayExecute() {
  ATH_MSG_DEBUG("TgcOverlay::execute() begin");

  //----------------------------------------------------------------

  SG::ReadHandle<TgcDigitContainer> dataContainer (m_mainInputDigitKey);
  if (!dataContainer.isValid()) {
    ATH_MSG_ERROR("Could not get data TGC container " << dataContainer.name() << " from store " << dataContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found data TgcDigitContainer called " << dataContainer.name() << " in store " << dataContainer.store());
  ATH_MSG_INFO("TGC Data     = "<<shortPrint(dataContainer.cptr()));

  ATH_MSG_VERBOSE("Retrieving MC  input TGC container");
  SG::ReadHandle<TgcDigitContainer> mcContainer(m_overlayInputDigitKey);
  if(!mcContainer.isValid() ) {
    ATH_MSG_ERROR("Could not get overlay TGC container " << mcContainer.name() << " from store " << mcContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found overlay TgcDigitContainer called " << mcContainer.name() << " in store " << mcContainer.store());
  ATH_MSG_INFO("TGC MC       = "<<shortPrint(mcContainer.cptr()));

  ATH_MSG_VERBOSE("TGC data has digit_size "<<dataContainer->digit_size());

  ATH_MSG_VERBOSE("TGC signal data has digit_size "<<mcContainer->digit_size());

  SG::WriteHandle<TgcDigitContainer> outputContainer(m_outputDigitKey);
  ATH_CHECK(outputContainer.record(std::make_unique<TgcDigitContainer>(dataContainer->size())));
  ATH_MSG_DEBUG("Recorded output TgcDigitContainer called " << outputContainer.name() << " in store " << outputContainer.store());

  //Do the actual overlay
  if(dataContainer.isValid() && mcContainer.isValid() && outputContainer.isValid()) {
    this->overlayContainer(dataContainer.cptr(), mcContainer.cptr(), outputContainer.ptr());
  }
  ATH_MSG_INFO("TGC Result   = "<<shortPrint(outputContainer.cptr()));

  //----------------------------------------------------------------
  ATH_MSG_DEBUG("TgcOverlay::execute() end");

  return StatusCode::SUCCESS;
}

// EOF
