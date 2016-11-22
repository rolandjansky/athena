/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2JetHypoNoise.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

//#include "TrigSteeringEvent/TriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigT2Jet.h"
//#include "TrigCaloEvent/TrigT2JetContainer.h" // Not needed anymore

#include "TrigJetHypo/TrigL2JetHypoNoise.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2JetHypoNoise::TrigL2JetHypoNoise(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("Etcut_L2",   m_EtCut_L2 = 20*CLHEP::GeV, "cut value for L2 jet et"); // Default: 20 GeV
  declareProperty("doMonitoring_L2", m_doMonitoring = false, "switch on/off monitoring" );
  declareProperty("AcceptAll",      m_acceptAll=false);
  
  // cleaning:
  // --------
  // flags to set which jet cleaning cuts are to be applied 
  declareProperty("doNLeadingCellsUncleaning", m_doNLeadingCellsUncleaning = true);
  declareProperty("doJetQualityUncleaning", m_doJetQualityUncleaning = true);
  declareProperty("leadingCellsThr", m_leadingCellsThr = 10);
  declareProperty("jetQualityThr", m_jetQualityThr= 0.6);

  declareMonitoredVariable("CutCounter", m_cutCounter);
  declareMonitoredVariable("E",   m_e);
  declareMonitoredVariable("Et",  m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
  // cleaning:
  declareMonitoredVariable("nLeadingCells",     m_nLeadingCells);
  declareMonitoredVariable("jetQuality",        m_jetQuality);
  
}
/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2JetHypoNoise::~TrigL2JetHypoNoise()
{ }

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2JetHypoNoise::hltInitialize()
// ----------------------------------------------------------------------
{
  msg() << MSG::INFO << "in initialize()" << endmsg;

  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    msg() << MSG::WARNING << name()
	<< ": Unable to locate TrigTimer Service" << endmsg;
  }
  if (m_timersvc) {

    TrigTimer* tmp = m_timersvc->addItem("TrigL2JetHypoNoise.TrigL2JetHypoTot");
    m_timers.push_back(tmp);
  }

  m_accepted_L2=0;
  m_rejected_L2=0;
  m_errors_L2=0;

  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2JetHypoNoise::hltFinalize(){
// ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events (Lvl2) accepted/rejected/errors:  "<< m_accepted_L2 <<" / "<<m_rejected_L2<< " / "<< m_errors_L2<< endmsg;
  
  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2JetHypoNoise::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
// ----------------------------------------------------------------------

  // Time total TrigL2JetHypoNoise execution time.

  if (m_timersvc) m_timers[0]->start();

  m_cutCounter = -1;
  m_e = -99;
  m_et = -99;
  m_eta = -99;
  m_phi = -99;
  m_nLeadingCells = -99;
  m_jetQuality = -99;

  pass=false;

  // get the trigger element and extract the RoI information
  std::vector<const TrigT2Jet*> vectorOfJets;

  HLT::ErrorCode ec = getFeatures(outputTE, vectorOfJets);

  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get the L2 Jets " << endmsg;
    if (m_timersvc) m_timers[0]->stop();
    return ec;
  }

  m_cutCounter = 0;

  // Check that there is only one JETROI.
  // We only expect ONE input RoI.
  if (vectorOfJets.size() != 1){
    msg() << MSG::ERROR << "The number of Jets attached to the TE is not 1" << endmsg;
    m_errors_L2++;
    if (m_acceptAll) m_cutCounter = 1;
    if (m_timersvc) m_timers[0]->stop();
    return HLT::OK;
  }

  // Get first (and only) RoI:
  const TrigT2Jet* pJet = vectorOfJets.back();
  if(!pJet){
    msg() << MSG::ERROR << "Retrieval of RoI from vector failed"  << endmsg;
    m_errors_L2++;
    if (m_acceptAll) m_cutCounter = 1;
    if (m_timersvc) m_timers[0]->stop();
    return HLT::OK;
  }

  //double etjet = pJet->e();
  double etjet = pJet->et();

  if(msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Jet energy (L2): " << etjet << endmsg;
	msg() << MSG::DEBUG << "Jet Leading Cells (L2): " << pJet->nLeadingCells() << endmsg;
	msg() << MSG::DEBUG << "Jet Quality (L2): " << pJet->jetQuality() << endmsg;
  }
  
  // ---------------------------------------------------------
  // jet Uncleaning:
  // =============
  // global flag cleaningVeto: if this flag is set to true, 
  // the hypo will reject the event (if in addition m_acceptAll == false)
  bool uncleaning = false;
  
  // n90 and hecfraction cut:
  if( m_doNLeadingCellsUncleaning && !m_doJetQualityUncleaning && pJet->nLeadingCells() >= m_leadingCellsThr  ) uncleaning = true;
  

  // bad quality 
  if( m_doJetQualityUncleaning && !m_doNLeadingCellsUncleaning && fabs(pJet->jetQuality()) > m_jetQualityThr ) uncleaning = true;

  // both
  if( m_doNLeadingCellsUncleaning && m_doJetQualityUncleaning && pJet->nLeadingCells() >= m_leadingCellsThr && fabs(pJet->jetQuality()) > m_jetQualityThr  )
	uncleaning = true;

  // end of jet cleaning
  // ---------------------------------------------------------

  // Et and Jet Quality Cut
  if( (etjet < -5000) || (etjet > m_EtCut_L2 && uncleaning ) || m_acceptAll) { 
    pass = true;
    m_accepted_L2++;
    m_cutCounter=1;

    if ( (etjet > m_EtCut_L2 && uncleaning == false) || ( etjet < -5000) ) {
      m_e = pJet->e();
      m_eta = pJet->eta();
      m_phi = pJet->phi();
      m_et = m_e/cosh(m_eta);
      // monitor cleaning variables:
      m_nLeadingCells = pJet->nLeadingCells();
      m_jetQuality = pJet-> jetQuality();
    }
    
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Noise Event accepted ! " << endmsg;
  } else {
    m_rejected_L2++;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Event rejected !" << endmsg;
  }

  //m_cutCounter>1 not yet implemented

  // Time total TrigL2JetHypo execution time.
  // -------------------------------------

  if (m_timersvc) m_timers[0]->stop();

  return HLT::OK;

}
