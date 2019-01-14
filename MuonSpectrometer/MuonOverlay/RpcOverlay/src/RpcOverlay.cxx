/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008

// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#include "RpcOverlay/RpcOverlay.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "GeneratorObjects/McEventCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonDigitContainer/RpcDigitContainer.h"

#include <iostream>
#include <typeinfo>


//================================================================
RpcOverlay::RpcOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  IDC_MultiHitOverlayBase(name, pSvcLocator)
{
  /** modifiable properties in job options */
  declareProperty("CopySDO", m_copySDO);
  declareProperty("RPC_SDO", m_sdo);
}

//================================================================
StatusCode RpcOverlay::overlayInitialize()
{
  ATH_MSG_INFO("RpcOverlay initialized");

  /** access to the CSC Identifier helper */
  ATH_CHECK(detStore()->retrieve(m_rpcHelper, "RPCIDHELPER"));
  ATH_MSG_DEBUG(" Found the RpcIdHelper. ");

  ATH_CHECK(m_mainInputDigitKey.initialize());
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_mainInputDigitKey );
  ATH_CHECK(m_overlayInputDigitKey.initialize());
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_overlayInputDigitKey );
  ATH_CHECK(m_outputDigitKey.initialize());
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputDigitKey );

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode RpcOverlay::overlayFinalize()
{
  ATH_MSG_INFO("RpcOverlay finalized");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode RpcOverlay::overlayExecute() {
  ATH_MSG_DEBUG("RpcOverlay::execute() begin");

  //----------------------------------------------------------------

  SG::ReadHandle<RpcDigitContainer> dataContainer (m_mainInputDigitKey);
  if (!dataContainer.isValid()) {
    ATH_MSG_ERROR("Could not get data RPC container " << dataContainer.name() << " from store " << dataContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found data RpcDigitContainer called " << dataContainer.name() << " in store " << dataContainer.store());
  ATH_MSG_INFO("RPC Data     = "<<shortPrint(dataContainer.cptr()));

  ATH_MSG_VERBOSE("Retrieving MC  input RPC container");
  SG::ReadHandle<RpcDigitContainer> mcContainer(m_overlayInputDigitKey);
  if(!mcContainer.isValid() ) {
    ATH_MSG_ERROR("Could not get overlay RPC container " << mcContainer.name() << " from store " << mcContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found overlay RpcDigitContainer called " << mcContainer.name() << " in store " << mcContainer.store());
  ATH_MSG_INFO("RPC MC       = "<<shortPrint(mcContainer.cptr()));

  ATH_MSG_VERBOSE("RPC data has digit_size "<<dataContainer->digit_size());

  ATH_MSG_VERBOSE("RPC signal data has digit_size "<<mcContainer->digit_size());

  SG::WriteHandle<RpcDigitContainer> outputContainer(m_outputDigitKey);
  ATH_CHECK(outputContainer.record(std::make_unique<RpcDigitContainer>(dataContainer->size())));
  ATH_MSG_DEBUG("Recorded output RpcDigitContainer called " << outputContainer.name() << " in store " << outputContainer.store());

  //Do the actual overlay
  if(dataContainer.isValid() && mcContainer.isValid() && outputContainer.isValid()) {
    this->overlayContainer(dataContainer.cptr(), mcContainer.cptr(), outputContainer.ptr());
  }
  ATH_MSG_INFO("RPC Result   = "<<shortPrint(outputContainer.cptr()));

  //----------------------------------------------------------------
  ATH_MSG_DEBUG("Processing MC truth data");

  // Main stream is normally real data without any MC info.
  // In tests we may use a MC generated file instead of real data.
  // Remove truth info from the main input stream, if any.
  //
  // Here we handle just RPC-specific truth classes.
  // (McEventCollection is done by the base.)

  // Now copy RPC-specific MC truth objects to the output.
  if ( m_copySDO )
    this->copyObjects<MuonSimDataCollection>(&*m_storeGateOutput, &*m_storeGateMC, m_sdo);

  //----------------------------------------------------------------
  ATH_MSG_DEBUG("RpcOverlay::execute() end");

  return StatusCode::SUCCESS;
}

// EOF
