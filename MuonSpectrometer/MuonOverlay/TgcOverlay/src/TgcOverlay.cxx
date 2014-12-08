/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008

// Piyali Banerjee <Piyali.Banerjee@cern.ch>, March 2011

#include "TgcOverlay/TgcOverlay.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "GeneratorObjects/McEventCollection.h"
#include "MuonSimData/MuonSimDataCollection.h"

#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonDigitContainer/TgcDigitContainer.h"

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

#include "AthenaKernel/getMessageSvc.h"

#include <iostream>
#include <typeinfo>

//================================================================
TgcOverlay::TgcOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  IDC_MultiHitOverlayBase(name, pSvcLocator),
  m_storeGateTemp("StoreGateSvc/BkgEvent_1_SG", name),
  m_storeGateTempBkg("StoreGateSvc/BkgEvent_2_SG", name),
  m_tgcHelper(0),
  m_digTool("MdtDigitizationTool", this ),
  m_rdoTool("MuonRdoToMuonDigitTool", this )
{

  /** Modifiable properties in job options */
  declareProperty("TempStore", m_storeGateTemp, "help");
  declareProperty("TempBkgStore", m_storeGateTempBkg, "help");
  declareProperty("mainInputTGC_Name", m_mainInputTGC_Name="tgc_digits");
  declareProperty("overlayInputTGC_Name", m_overlayInputTGC_Name="tgc_digits");
  declareProperty("CopySDO", m_copySDO=true);

  declareProperty("DigitizationTool", m_digTool);
  declareProperty("ConvertRDOToDigitTool", m_rdoTool);

  declareProperty("TGCSDO", m_sdo = "TGC_SDO");

  declareProperty("CopyObject", m_copyObjects=false); 
}

//================================================================
StatusCode TgcOverlay::overlayInitialize()
{
  msg( MSG::INFO ) << "TgcOverlay initialized" << endreq;

  if (m_storeGateTemp.retrieve().isFailure()) {
     ATH_MSG_FATAL("TgcOverlay::initialize(): TempStore for signal not found !");
     return StatusCode::FAILURE;
  }

  if (m_storeGateTempBkg.retrieve().isFailure()) {
     ATH_MSG_FATAL("TgcOverlay::initialize(): TempStoreBkg not found !");
     return StatusCode::FAILURE;
  }

  /** initialize the detectore store service */
  StoreGateSvc* detStore=0;
  StatusCode sc = serviceLocator()->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  }

  /** access to the TGC Identifier helper */
  sc = detStore->retrieve(m_tgcHelper, "TGCIDHELPER");
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "Could not get TgcIdHelper !" << endreq;
    return StatusCode::FAILURE;
  } 
  else {
    msg( MSG::DEBUG ) << " Found the TgcIdHelper. " << endreq;
  }

  if (m_digTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve TGC Digitization Tool!"
                      << endreq;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved TGC Digitization Tool." << endreq;
  
  if (m_rdoTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve TGC RDO -> Digit Tool!"
                      << endreq;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved TGC RDO -> Digit Tool." << endreq;

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode TgcOverlay::overlayFinalize() 
{
  msg( MSG::INFO ) << "TgcOverlay finalized" << endreq;
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode TgcOverlay::overlayExecute() {
  msg( MSG::DEBUG ) << "TgcOverlay::execute() begin"<< endreq;

  //----------------------------------------------------------------
  /** In the real data stream, run RDO -> Digit converter to make Digit
      this will be used in the overlay job */
  if ( m_rdoTool->digitize().isFailure() ) {
    msg( MSG::ERROR ) << "On the fly TGC RDO -> Digit failed " << endreq;
    return StatusCode::FAILURE;
  }

  /** in the simulation stream, run digitization of the fly
      and make Digit - this will be used as input to the overlay job */
  if ( m_digTool->digitize().isFailure() ) {
    msg( MSG::ERROR ) << "On the fly TGC digitization failed " << endreq;
    return StatusCode::FAILURE;
  }

  /** save a copy of the TGC Digit Container in a temp store */
  if ( m_copyObjects )
     this->copyMuonIDCobject<TgcDigitContainer,TgcDigit>(&*m_storeGateMC,&*m_storeGateTemp);

  std::auto_ptr<TgcDigitContainer> tgc(m_storeGateData->retrievePrivateCopy<TgcDigitContainer>(m_mainInputTGC_Name));
  if ( !tgc.get() ) {
    msg( MSG::ERROR ) << "Could not get data TGC container " << m_mainInputTGC_Name << endreq;
    return StatusCode::FAILURE;
  }

  //log << MSG::DEBUG << "TGC Data   = " << this->shortPrint(cdata) << endreq;

  msg( MSG::VERBOSE ) << "Retrieving MC input TGC container" << endreq;
  std::auto_ptr<TgcDigitContainer> ovl_input_TGC(m_storeGateMC->retrievePrivateCopy<TgcDigitContainer>(m_overlayInputTGC_Name));
  if(!ovl_input_TGC.get() ) {
    msg( MSG::ERROR ) << "Could not get overlay TGC container " << m_overlayInputTGC_Name << endreq;
    return StatusCode::FAILURE;
  }
  //log << MSG::DEBUG << "TGC MC     = " << this->shortPrint(ovl_input_TGC) << endreq;

  TgcDigitContainer *tgc_temp_bkg = copyMuonDigitContainer<TgcDigitContainer,TgcDigit>(tgc.get());

  if ( m_storeGateTempBkg->record(tgc_temp_bkg, m_mainInputTGC_Name).isFailure() ) {
     msg( MSG::WARNING ) << "Failed to record background TgcDigitContainer to temporary background store " << endreq;
  }

  this->overlayContainer(tgc, ovl_input_TGC);
  //log << MSG::DEBUG << "TGC Result = " << this->shortPrint(cdata) << endreq;

  if ( m_storeGateOutput->record(tgc, m_mainInputTGC_Name).isFailure() )
    msg( MSG::WARNING ) << "Failed to record TGC overlay container to output store " << endreq;

  //----------------
  // This kludge is a work around for problems created by another kludge:
  // Digitization algs keep a pointer to their output Identifiable Container and reuse
  // the same object over and other again.   So unlike any "normal" per-event object
  // this IDC is not a disposable one, and we should not delete it.
  ovl_input_TGC.release();
  tgc.release();

  //----------------------------------------------------------------
  msg(MSG::DEBUG ) <<"Processing MC truth data"<<endreq;

  // Main stream is normally real data without any MC info.
  // In tests we may use a MC generated file instead of real data.
  // Remove truth info from the main input stream, if any.
  //
  // Here we handle just TGC-specific truth classes.
  // (McEventCollection is done by the base.)

  // Now copy TGC-specific MC truth objects to the output.
  if ( m_copySDO )
    this->copyObjects<MuonSimDataCollection>(&*m_storeGateOutput, &*m_storeGateMC, m_sdo);

  //----------------------------------------------------------------
  msg( MSG::DEBUG ) << "TgcOverlay::execute() end"<< endreq;

  return StatusCode::SUCCESS; 
}

// EOF

  
