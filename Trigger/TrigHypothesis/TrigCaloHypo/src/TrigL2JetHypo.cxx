/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2JetHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigCaloHypo
//
// AUTHOR:
//
// ********************************************************************

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SystemOfUnits.h"

//#include "TrigSteeringEvent/TriggerElement.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigT2Jet.h"
//#include "TrigCaloEvent/TrigT2JetContainer.h" // Not needed anymore

#include "TrigL2JetHypo.h"


class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2JetHypo::TrigL2JetHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("Etcut_L2",   m_EtCut_L2 = 20*Gaudi::Units::GeV, "cut value for L2 jet et"); // Default: 20 GeV
  declareProperty("doMonitoring_L2", m_doMonitoring = false, "switch on/off monitoring" );
  declareProperty("AcceptAll",      m_acceptAll=false);
  //declareProperty("histoPath", m_path = "/stat/Monitoring/EventFilter" );
  
  // cleaning:
  // --------
  // flags to set which jet cleaning cuts are to be applied 
  declareProperty("doHECfnLeadingCellsCleaning", m_doHECfnLeadingCellsCleaning = false);
  declareProperty("doEMfJetQualityCleaning",     m_doEMfJetQualityCleaning = false);
  declareProperty("doHECfJetQualityCleaning",    m_doHECfJetQualityCleaning = false);
  declareProperty("doTimeQualityCleaning",       m_doTimeQualityCleaning = false);
  declareProperty("doEMfCleaningHighEta",        m_doEMfCleaningHighEta = false);
  declareProperty("doEMfCleaningLowEta",         m_doEMfCleaningLowEta = false);
  declareProperty("applyCleaningToHighEtJets",   m_applyCleaningToHighEtJets = true);
  declareProperty("applyCleaningToLowEtJets",    m_applyCleaningToLowEtJets = true);
  // Et-threshold: if(applyCleaningToHighEtJets==false) then don't apply cleaning cuts to jets with Et > highEtThreshold:
  declareProperty("cleaningHighEtThreshold",     m_cleaningHighEtThreshold = 1000.*Gaudi::Units::GeV);
  // Et-threshold: if(applyCleaningToLowEtJets==false) then don't apply cleaning cuts to jets with Et < lowEtThreshold:
  declareProperty("cleaningLowEtThreshold",      m_cleaningLowEtThreshold = 20.*Gaudi::Units::GeV);
  // threshold for number of leading cells in hecf > m_leadingCellsThr &&  nLeadingCells <= m_leadingCellsThr cut:
  declareProperty("leadingCellsThr", m_leadingCellsThr = 1);
  // hecf threshold for cut in combination with nLeadingCells (i.e. hecf > m_hecfThrN &&  nLeadingCells <= m_leadingCellsThr):
  declareProperty("hecfThrN", m_hecfThrN = 0.9);
  // hecf threshold for cut in combination with jet Quality (i.e. hecf > m_hecfThrQ && |quality| > m_jetQualityThrH):
  declareProperty("hecfThrQ", m_hecfThrQ = 0.8);
  // quality threshold for cut in combination with hecf (i.e. hecf > m_hecfThrQ && |quality| > m_jetQualityThrH):
  declareProperty("jetQualityThrH", m_jetQualityThrH= 0.8);
  // quality threshold for cut in combination with emf (i.e. emf > m_emfThrQ && |quality| > m_jetQualityThrE):
  declareProperty("jetQualityThrE", m_jetQualityThrE = 0.9);
   // emf threshold for cut in combination with m_jetQualityThrE (i.e. emf > m_emfThrQ && |quality| > m_jetQualityThrE):
  declareProperty("emfThrQ", m_emfThrQ = 0.99);
  // eta threshold for cut in combination with m_jetQualityThrE (i.e. emf > m_emfThrQ && |quality| > m_jetQualityThrE && |eta| < m_etaThrEmfQ)
  declareProperty("etaThrEmfQ", m_etaThrEmfQ = 2.7);
  // emf threshold for emf cut at high eta (i.e. emf < m_emfThrE && |eta| >= m_etaThrEemf):
  declareProperty("emfThrEmfHighEta", m_emfThrEmfHighEta = 0.02);
  // eta threshold for emf cut at high eta (i.e. emf < m_emfThrE && |eta| >= m_etaThrEemf):
  declareProperty("etaThrEmfHighEta", m_etaThrEmfHighEta = 2.1);
  // emf threshold for emf cut at low eta (i.e. emf < m_emfThrE && |eta| < m_etaThrEemf):
  declareProperty("emfThrEmfLowEta", m_emfThrEmfLowEta = 0.01);
  // eta threshold for emf cut at low eta (i.e. emf < m_emfThrE && |eta| < m_etaThrEemf):
  declareProperty("etaThrEmfLowEta", m_etaThrEmfLowEta = 2.1);
  // time threshold for identification of out-of-time jets (i.e. |time| > m_timeThrTimeQuality && |quality| > m_qualityThrTimeQuality):
  declareProperty("timeThrTimeQuality", m_timeThrTimeQuality = 35.);
  // quality threshold for identification of out-of-time jets (i.e. |time| > m_timeThrTimeQuality && |quality| > m_qualityThrTimeQuality):
  declareProperty("qualityThrTimeQuality", m_qualityThrTimeQuality = 0.5);
  
  // basic cleaning
  declareProperty("doBasicCleaning", m_doBasicCleaning = false );
  
  declareProperty("n90CleaningThreshold", m_n90Threshold = 2 );
  declareProperty("presamplerCleaningThreshold", m_presamplerThreshold = 0.9 );
  declareProperty("negativeECleaningThreshold", m_negativeEThreshold = -60e3 ); // 60 GeV


  declareMonitoredVariable("CutCounter", m_cutCounter);
  declareMonitoredVariable("E",   m_e);
  declareMonitoredVariable("Et",  m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
  // cleaning:
  declareMonitoredVariable("HECf",              m_hecf);
  declareMonitoredVariable("nLeadingCells",     m_nLeadingCells);
  declareMonitoredVariable("EMf",               m_emf);
  declareMonitoredVariable("jetQuality",        m_jetQuality);
  declareMonitoredVariable("jetTimeCells",      m_jetTimeCells);
  
}
/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2JetHypo::~TrigL2JetHypo()
{ }

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2JetHypo::hltInitialize()
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

    TrigTimer* tmp = m_timersvc->addItem("TrigL2JetHypo.TrigL2JetHypoTot");
    m_timers.push_back(tmp);
  }

  m_accepted_L2=0;
  m_rejected_L2=0;
  m_errors_L2=0;

  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2JetHypo::hltFinalize(){
// ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events (Lvl2) accepted/rejected/errors:  "<< m_accepted_L2 <<" / "<<m_rejected_L2<< " / "<< m_errors_L2<< endmsg;
  
  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2JetHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
// ----------------------------------------------------------------------

  // Time total TrigL2JetHypo execution time.

  if (m_timersvc) m_timers[0]->start();

  m_cutCounter = -1;
  m_e = -99;
  m_et = -99;
  m_eta = -99;
  m_phi = -99;
  m_hecf = -99;
  m_nLeadingCells = -99;
  m_emf = -99;
  m_jetQuality = -99;
  m_jetTimeCells = -999;

  pass=false;

  // get the trigger element and extract the RoI information

  if(msgLvl() <= MSG::DEBUG) { // pure debug purposes...
    const TrigRoiDescriptor* roiDescriptor = 0;
    if ( getFeature(outputTE, roiDescriptor, "a_label")==HLT::OK ) {
      if ( roiDescriptor ) {
        msg() << MSG::DEBUG << "REGTEST: RoI id " << roiDescriptor->roiId()
            << " located at   phi = " <<  roiDescriptor->phi() << ", eta = " << roiDescriptor->eta() << endmsg;
      } else {
        msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endmsg;
      }
    } else {
      msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endmsg;
    }
  }

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

  // Build the "uses" relation for the outputTE to the Cluster:
  //sc = writeHistoryOfTE(outputTE, uses, pJet);
  /*if (!sc){
    log << MSG::ERROR
        << "Write of TrigT2Jet into outputTE failed"
        << endmsg;
    return StatusCode::FAILURE;
  } */ // Not used anymore.

  //double etjet = pJet->e();
  double etjet = pJet->et();

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jet energy (L2): " << etjet << endmsg;
  
  // ---------------------------------------------------------
  // jet cleaning:
  // =============
  // global flag cleaningVeto: if this flag is set to true, 
  // the hypo will reject the event (if in addition m_acceptAll == false)
  bool cleaningVeto = false;
  
  // depending on the values of the two flags applyCleaningToHighEtJets and applyCleaningToLowEtJets, the cleaning
  // is only applied to jets with Et > cleaningLowEtThreshold and/or Et < cleaningHighEtThreshold.
  bool applyCleaning = true;
  
  if(m_applyCleaningToHighEtJets == false && etjet > m_cleaningHighEtThreshold){
    applyCleaning = false;
  }
  else if(m_applyCleaningToLowEtJets == false && etjet < m_cleaningLowEtThreshold){   // else if construct for performance
    applyCleaning = false;
  }

  if(applyCleaning == true){
    // n90 and hecfraction cut:
    if( m_doHECfnLeadingCellsCleaning == true && pJet->nLeadingCells() <= m_leadingCellsThr && pJet->hecf() > m_hecfThrN ){
     cleaningVeto = true;
    }
    // else if constructs for performance (once cleaning veto is true, the decision to reject the jet is taken)
    // bad quality in EMC:
    else if( m_doEMfJetQualityCleaning == true && 
             pJet->emf() > m_emfThrQ && fabs(pJet->jetQuality()) > m_jetQualityThrE && fabs(pJet->eta()) < m_etaThrEmfQ ){
      cleaningVeto = true;
    }
    // bad quality in HEC
    else if( m_doHECfJetQualityCleaning == true && pJet->hecf() > m_hecfThrQ && fabs(pJet->jetQuality()) > m_jetQualityThrH ){
      cleaningVeto = true;
    }
    // out-of-time jets
    else if( m_doTimeQualityCleaning == true && 
             fabs(pJet->jetTimeCells()) > m_timeThrTimeQuality && fabs(pJet->jetQuality()) > m_qualityThrTimeQuality ){
      cleaningVeto = true;
    }  
    // very small fraction of energy in EMC at high eta:
    else if(m_doEMfCleaningHighEta == true && pJet->emf() < m_emfThrEmfHighEta && fabs(pJet->eta()) >= m_etaThrEmfHighEta ){
      cleaningVeto = true;
    }
    // very small fraction of energy in EMC at low eta:
    else if(m_doEMfCleaningLowEta == true && pJet->emf() < m_emfThrEmfLowEta && fabs(pJet->eta()) < m_etaThrEmfLowEta ){
      cleaningVeto = true;
    }
  }
  
  if(m_doBasicCleaning)
  {
    if(pJet->nLeadingCells() < m_n90Threshold)
    {
        msg() << MSG::DEBUG << " L2 basic jet cleaning: n90 < " << m_n90Threshold << endmsg;
        cleaningVeto = true;
    }
    
/*    if(pJet->presamplerFraction() > m_presamplerThreshold)
    {
        msg() << MSG::DEBUG << " L2 basic jet cleaning: presampler fraction > " << m_presamplerThreshold << endmsg;
        cleaningVeto = true;
    }
    
    if(pJet->?() < m_negativeEThreshold)
    {
        msg() << MSG::DEBUG << " EF jet cleaning: Negative energy < " << m_negativeEThreshold << endmsg;
        cleaningVeto = true;
    }*/
  }
  // end of jet cleaning
  // ---------------------------------------------------------

  // Et and Jet Quality Cut
  if( (etjet > m_EtCut_L2 && cleaningVeto == false) || m_acceptAll) { 
    pass = true;
    m_accepted_L2++;
    m_cutCounter=1;

    if ( etjet > m_EtCut_L2 && cleaningVeto == false) {
      m_e = pJet->e();
      m_eta = pJet->eta();
      m_phi = pJet->phi();
      m_et = m_e/cosh(m_eta);
      // monitor cleaning variables:
      m_emf = pJet->emf();
      m_hecf = pJet->hecf();
      m_nLeadingCells = pJet->nLeadingCells();
      m_jetQuality = pJet-> jetQuality();
      m_jetTimeCells = pJet-> jetTimeCells(); 
    }
    
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Event accepted ! " << endmsg;
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
