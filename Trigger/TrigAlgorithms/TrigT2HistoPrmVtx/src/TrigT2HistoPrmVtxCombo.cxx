/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigT2HistoPrmVtxCombo.cxx
// PACKAGE:  Trigger/TrigAlgorithm/TrigT2HistoPrmVtx
//
// AUTHOR:   Viviana Cavaliere viviana.cavaliere@cern.ch
//
// ********************************************************************


#include "GaudiKernel/IToolSvc.h"

#include "TrigT2HistoPrmVtx/TrigT2HistoPrmVtxCombo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigNavigation/TriggerElement.h"

class ISvcLocator;

TrigT2HistoPrmVtxCombo::TrigT2HistoPrmVtxCombo(const std::string & name, ISvcLocator* pSvcLocator)
 : HLT::ComboAlgo(name, pSvcLocator)
{

  //Monitor
  declareProperty("doMonitoring_L2", m_doMonitoring = true, "switch on/off monitoring" );
  declareMonitoredVariable("m_nTeCounter", m_nTeCounter);
  
}

HLT::ErrorCode TrigT2HistoPrmVtxCombo::hltInitialize()
{
  return HLT::OK;
}

TrigT2HistoPrmVtxCombo::~TrigT2HistoPrmVtxCombo(){
}

HLT::ErrorCode TrigT2HistoPrmVtxCombo::acceptInputs(HLT::TEConstVec& inputTE, bool& pass ) {

  // Time total TrigT2HistoPrmVtxCombo execution time.
  

  pass=false;
  m_nTeCounter = -1; // Monitoring variable

  // sanity check there has to be as many input Te as ROIS
  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigT2HistoPrmVtxCombo::acceptInputs, got: " << inputTE.size() << " TE as input " << endreq;

  m_nTeCounter = inputTE.size(); // Monitoring variable

  if ( inputTE.size() != 2) {
    msg() << MSG::WARNING << "Got less than 2 input TE " << m_nTeCounter << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE); 
  }
  
  pass=true;


  return HLT::OK;
}

HLT::ErrorCode TrigT2HistoPrmVtxCombo::hltExecute(HLT::TEConstVec& /*inputTE*/, HLT::TriggerElement* /*outputTE*/){

  if ( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << "Running TrigT2HistoPrmVtxCombo::hltExecute" << endreq;

  return HLT::OK;
}

HLT::ErrorCode TrigT2HistoPrmVtxCombo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  return HLT::OK;
}



