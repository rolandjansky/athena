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

  declareProperty("DigitizationTool", m_digTool);
  declareProperty("ConvertRDOToDigitTool", m_rdoTool);

  declareProperty("CopyObject", m_copyObjects=false); 
}

//================================================================
StatusCode TgcOverlay::overlayInitialize()
{
  msg( MSG::INFO ) << "TgcOverlay initialized" << endmsg;

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
    msg( MSG::FATAL ) << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  }

  /** access to the TGC Identifier helper */
  sc = detStore->retrieve(m_tgcHelper, "TGCIDHELPER");
  if (sc.isFailure()) {
    msg( MSG::FATAL ) << "Could not get TgcIdHelper !" << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    msg( MSG::DEBUG ) << " Found the TgcIdHelper. " << endmsg;
  }

  if (m_digTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve TGC Digitization Tool!"
                      << endmsg;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved TGC Digitization Tool." << endmsg;
  
  if (m_rdoTool.retrieve().isFailure()) {
    msg( MSG::FATAL ) << "Could not retrieve TGC RDO -> Digit Tool!"
                      << endmsg;
    return StatusCode::FAILURE;
  }
  msg( MSG::DEBUG ) << "Retrieved TGC RDO -> Digit Tool." << endmsg;

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode TgcOverlay::overlayFinalize() 
{
  msg( MSG::INFO ) << "TgcOverlay finalized" << endmsg;
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode TgcOverlay::overlayExecute() {
  msg( MSG::DEBUG ) << "TgcOverlay::execute() begin"<< endmsg;

  //----------------------------------------------------------------
  /** In the real data stream, run RDO -> Digit converter to make Digit
      this will be used in the overlay job */
  if ( m_rdoTool->digitize().isFailure() ) {
    msg( MSG::ERROR ) << "On the fly TGC RDO -> Digit failed " << endmsg;
    return StatusCode::FAILURE;
  }

  /** in the simulation stream, run digitization of the fly
      and make Digit - this will be used as input to the overlay job */
  if ( m_digTool->digitize().isFailure() ) {
    msg( MSG::ERROR ) << "On the fly TGC digitization failed " << endmsg;
    return StatusCode::FAILURE;
  }

  /** save a copy of the TGC Digit Container in a temp store */
  if ( m_copyObjects )
     this->copyMuonIDCobject<TgcDigitContainer,TgcDigit>(&*m_storeGateMC,&*m_storeGateTemp);

  SG::ReadHandle<TgcDigitContainer> dataContainer(m_mainInputTGC_Name, m_storeGateData->name());
  if ( !dataContainer.isValid() ) {
    msg( MSG::ERROR ) << "Could not get data TGC container " << m_mainInputTGC_Name << endmsg;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("TGC Data   = "<<shortPrint(dataContainer.cptr()));

  msg( MSG::VERBOSE ) << "Retrieving MC input TGC container" << endmsg;
  SG::ReadHandle<TgcDigitContainer> mcContainer(m_overlayInputTGC_Name, m_storeGateMC->name()); 
  if(!mcContainer.isValid() ) {
    msg( MSG::ERROR ) << "Could not get overlay TGC container " << m_overlayInputTGC_Name << endmsg;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("TGC MC   = "<<shortPrint(mcContainer.cptr()));

  /*  TgcDigitContainer *tgc_temp_bkg = copyMuonDigitContainer<TgcDigitContainer,TgcDigit>(dataContainer.cptr());
  if ( m_storeGateTempBkg->record(tgc_temp_bkg, m_mainInputTGC_Name).isFailure() ) {
     msg( MSG::WARNING ) << "Failed to record background TgcDigitContainer to temporary background store " << endmsg;
     }*/

  SG::WriteHandle<TgcDigitContainer> outputContainer(m_mainInputTGC_Name, m_storeGateOutput->name());
  outputContainer = std::make_unique<TgcDigitContainer>(dataContainer->size());
  //Do the actual overlay
  if(dataContainer.isValid() && mcContainer.isValid() && outputContainer.isValid()) { 
    this->overlayContainer(dataContainer.cptr(), mcContainer.cptr(), outputContainer.ptr());
  }
  ATH_MSG_INFO("TGC Result   = "<<shortPrint(outputContainer.cptr()));

  //----------------------------------------------------------------
  msg( MSG::DEBUG ) << "TgcOverlay::execute() end"<< endmsg;

  return StatusCode::SUCCESS; 
}

// EOF

  
