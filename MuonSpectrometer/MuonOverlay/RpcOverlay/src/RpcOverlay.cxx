/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008

// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#include "RpcOverlay/RpcOverlay.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonDigitContainer/RpcDigitContainer.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

#include <iostream>
#include <typeinfo>


//================================================================
RpcOverlay::RpcOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  IDC_MultiHitOverlayBase(name, pSvcLocator),
  m_storeGateTemp("StoreGateSvc/BkgEvent_1_SG", name),
  m_storeGateTempBkg("StoreGateSvc/BkgEvent_2_SG", name),
  m_rpcHelper(0),
  m_digTool("MdtDigitizationTool", this ),
  m_rdoTool("MuonRdoToMuonDigitTool", this )

{
  /** modifiable properties in job options */
  declareProperty("TempStore", m_storeGateTemp, "help");
  declareProperty("TempBkgStore", m_storeGateTempBkg, "help");
  declareProperty("mainInputRPC_Name", m_mainInputRPC_Name="rpc_digits");
  declareProperty("overlayInputRPC_Name", m_overlayInputRPC_Name="rpc_digits");
  declareProperty("DigitizationTool", m_digTool);
  declareProperty("ConvertRDOToDigitTool", m_rdoTool);
  declareProperty("CopyObject", m_copyObjects=false);
}

//================================================================
StatusCode RpcOverlay::overlayInitialize()
{
  msg( MSG::INFO ) << "RpcOverlay initialized" << endmsg;

  if (m_storeGateTemp.retrieve().isFailure()) {
     ATH_MSG_FATAL("RpcOverlay::initialize(): TempStore for signal not found !");
     return StatusCode::FAILURE;
  }
  
  if (m_storeGateTempBkg.retrieve().isFailure()) {
     ATH_MSG_FATAL("RpcOverlay::initialize(): TempStoreBkg not found !");
     return StatusCode::FAILURE;
  }

  /** initialize the detectore store service */
  StoreGateSvc* detStore=0;
  StatusCode sc = serviceLocator()->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  }

  /** access to the CSC Identifier helper */
  sc = detStore->retrieve(m_rpcHelper, "RPCIDHELPER");
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "Could not get RpcIdHelper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    msg( MSG::DEBUG ) << " Found the RpcIdHelper. " << endmsg;
  }

  if (m_digTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve RPC Digitization Tool!"
        << endmsg;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved RPC Digitization Tool." << endmsg;
  
  if (m_rdoTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve RPC RDO -> Digit Tool!"
                      << endmsg;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved RPC RDO -> Digit Tool." << endmsg;

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode RpcOverlay::overlayFinalize() 
{
  msg( MSG::INFO ) << "RpcOverlay finalized" << endmsg;
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode RpcOverlay::overlayExecute() {
  msg( MSG::DEBUG ) << "RpcOverlay::execute() begin"<< endmsg;

  //----------------------------------------------------------------

  /** In the real data stream, run RDO -> Digit converter to make Digit
      this will be used in the overlay job */
  if ( m_rdoTool->digitize().isFailure() ) {
     msg( MSG::ERROR ) << "On the fly RPC RDO -> Digit failed " << endmsg;
     return StatusCode::FAILURE;
  }

  /** in the simulation stream, run digitization of the fly
      and make Digit - this will be used as input to the overlay job */
  if ( m_digTool->digitize().isFailure() ) {
     msg( MSG::ERROR ) << "On the fly RPC digitization failed " << endmsg;
     return StatusCode::FAILURE;
  }

  /** save a copy of the RPC Digit Container in a temp store */
  if ( m_copyObjects ) 
     this->copyMuonIDCobject<RpcDigitContainer,RpcDigit>(&*m_storeGateMC,&*m_storeGateTemp);

  SG::ReadHandle<RpcDigitContainer> dataContainer(m_mainInputRPC_Name, m_storeGateData->name());
   if ( !dataContainer.isValid() ) {
     msg( MSG::ERROR ) << "Could not get data RPC container " << m_mainInputRPC_Name << endmsg;
     return StatusCode::FAILURE;
  }
   ATH_MSG_INFO("RPC Data   = "<<shortPrint(dataContainer.cptr()));

  msg( MSG::VERBOSE ) << "Retrieving MC input RPC container" << endmsg;
  SG::ReadHandle<RpcDigitContainer> mcContainer(m_overlayInputRPC_Name, m_storeGateMC->name());
  if(!mcContainer.isValid()) {
    msg( MSG::ERROR ) << "Could not get overlay RPC container " << m_overlayInputRPC_Name << endmsg;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("RPC MC   = "<<shortPrint(mcContainer.cptr()));

  /* RpcDigitContainer *rpc_temp_bkg = copyMuonDigitContainer<RpcDigitContainer,RpcDigit>(dataContainer.cptr());

  if ( m_storeGateTempBkg->record(rpc_temp_bkg, m_mainInputRPC_Name).isFailure() ) {
     msg( MSG::WARNING ) << "Failed to record background RpcDigitContainer to temporary background store " << endmsg;
     }*/

  SG::WriteHandle<RpcDigitContainer> outputContainer(m_mainInputRPC_Name, m_storeGateOutput->name());
  outputContainer = std::make_unique<RpcDigitContainer>(dataContainer->size());
  //Do the actual overlay
  if(dataContainer.isValid() && mcContainer.isValid() && outputContainer.isValid()) { 
    this->overlayContainer(dataContainer.cptr(), mcContainer.cptr(), outputContainer.ptr());
  }
  ATH_MSG_INFO("RPC Result   = "<<shortPrint(outputContainer.cptr()));

  //----------------------------------------------------------------
  msg( MSG::DEBUG ) << "RpcOverlay::execute() end"<< endmsg;

  return StatusCode::SUCCESS;
}

// EOF
