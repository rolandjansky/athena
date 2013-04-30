/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008

// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#include "RpcOverlay/RpcOverlay.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GeneratorObjects/McEventCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"

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
  m_digTool("MdtDigitizationTool", this ),
  m_rdoTool("MuonRdoToMuonDigitTool", this )

{
  /** modifiable properties in job options */
  declareProperty("TempStore", m_storeGateTemp, "help");
  declareProperty("TempBkgStore", m_storeGateTempBkg, "help");
  declareProperty("mainInputRPC_Name", m_mainInputRPC_Name="rpc_digits");
  declareProperty("overlayInputRPC_Name", m_overlayInputRPC_Name="rpc_digits");
  declareProperty("CopySDO", m_copySDO=true);
  declareProperty("DigitizationTool", m_digTool);
  declareProperty("ConvertRDOToDigitTool", m_rdoTool);
  declareProperty("RPC_SDO", m_sdo = "RPC_SDO");
  declareProperty("CopyObject", m_copyObjects=false);
}

//================================================================
StatusCode RpcOverlay::overlayInitialize()
{
  msg( MSG::INFO ) << "RpcOverlay initialized" << endreq;

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
    msg( MSG::FATAL ) << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  }

  /** access to the CSC Identifier helper */
  sc = detStore->retrieve(m_rpcHelper, "RPCIDHELPER");
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "Could not get RpcIdHelper !" << endreq;
    return StatusCode::FAILURE;
  } 
  else {
    msg( MSG::DEBUG ) << " Found the RpcIdHelper. " << endreq;
  }

  if (m_digTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve RPC Digitization Tool!"
        << endreq;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved RPC Digitization Tool." << endreq;
  
  if (m_rdoTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve RPC RDO -> Digit Tool!"
                      << endreq;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved RPC RDO -> Digit Tool." << endreq;

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode RpcOverlay::overlayFinalize() 
{
  msg( MSG::INFO ) << "RpcOverlay finalized" << endreq;
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode RpcOverlay::overlayExecute() {
  msg( MSG::DEBUG ) << "RpcOverlay::execute() begin"<< endreq;

  //----------------------------------------------------------------

  /** In the real data stream, run RDO -> Digit converter to make Digit
      this will be used in the overlay job */
  if ( m_rdoTool->digitize().isFailure() ) {
     msg( MSG::ERROR ) << "On the fly RPC RDO -> Digit failed " << endreq;
     return StatusCode::FAILURE;
  }

  /** in the simulation stream, run digitization of the fly
      and make Digit - this will be used as input to the overlay job */
  if ( m_digTool->digitize().isFailure() ) {
     msg( MSG::ERROR ) << "On the fly RPC digitization failed " << endreq;
     return StatusCode::FAILURE;
  }

  /** save a copy of the RPC Digit Container in a temp store */
  if ( m_copyObjects ) 
     this->copyMuonIDCobject<RpcDigitContainer,RpcDigit>(&*m_storeGateMC,&*m_storeGateTemp);

  std::auto_ptr<RpcDigitContainer> rpc(m_storeGateData->retrievePrivateCopy<RpcDigitContainer>(m_mainInputRPC_Name));
  if ( !rpc.get() ) {
     msg( MSG::ERROR ) << "Could not get data RPC container " << m_mainInputRPC_Name << endreq;
     return StatusCode::FAILURE;
  }

  msg( MSG::VERBOSE ) << "Retrieving MC input RPC container" << endreq;
  std::auto_ptr<RpcDigitContainer> ovl_input_RPC(m_storeGateMC->retrievePrivateCopy<RpcDigitContainer>(m_overlayInputRPC_Name));
  if(!ovl_input_RPC.get()) {
    msg( MSG::ERROR ) << "Could not get overlay RPC container " << m_overlayInputRPC_Name << endreq;
    return StatusCode::FAILURE;
  }
  //log << MSG::DEBUG << "RPC MC     = " << this->shortPrint(ovl_input_RPC) << endreq;

  RpcDigitContainer *rpc_temp_bkg = copyMuonDigitContainer<RpcDigitContainer,RpcDigit>(rpc.get());

  if ( m_storeGateTempBkg->record(rpc_temp_bkg, m_mainInputRPC_Name).isFailure() ) {
     msg( MSG::WARNING ) << "Failed to record background RpcDigitContainer to temporary background store " << endreq;
  }

  this->overlayContainer(rpc, ovl_input_RPC);
  //log << MSG::DEBUG << "RPC Result = " << this->shortPrint(cdata) << endreq;

  if ( m_storeGateOutput->record(rpc, m_mainInputRPC_Name).isFailure() )
    msg( MSG::WARNING ) << "Failed to record RPC overlay container to output store " << endreq;

  //----------------
  // This kludge is a work around for problems created by another kludge:
  // Digitization algs keep a pointer to their output Identifiable Container and reuse
  // the same object over and other again.   So unlike any "normal" per-event object
  // this IDC is not a disposable one, and we should not delete it.
  ovl_input_RPC.release();
  rpc.release();

  //----------------------------------------------------------------
  msg( MSG::DEBUG ) <<"Processing MC truth data"<<endreq;

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
  msg( MSG::DEBUG ) << "RpcOverlay::execute() end"<< endreq;

  return StatusCode::SUCCESS;
}

// EOF

