/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2HVJetHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigLongLivedParticlesHypo
//
// AUTHORS: Stefano.Giagu@cern.ch; Antonio.Policicchio@cern.ch
//
// ********************************************************************

#include <math.h>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigLongLivedParticlesHypo/TrigL2HVJetHypo.h"

#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

#include "CxxUtils/fpcompare.h"



class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2HVJetHypo::TrigL2HVJetHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("Etcut_L2",            m_EtCut_L2 = 35*Gaudi::Units::GeV, "cut value for L2 jet et");
  declareProperty("LRaticout_L2",        m_LRatioCut_L2 = 1., "cut value for L2 jet log10 of had over em energy ratio"); 
  declareProperty("doMonitoring_L2",     m_doMonitoring = true, "switch on/off monitoring" );
  declareProperty("AcceptAll",           m_acceptAll=false);
  declareProperty("JetEta",              m_JetEta = 2.5, "cut value for Eta of the Jet");  


  // cleaning:
  // --------
  // flags to set which jet cleaning cuts are to be applied
  declareProperty("doHECfnLeadingCellsCleaning", m_doHECfnLeadingCellsCleaning   = true);
  declareProperty("doEMfJetQualityCleaning",     m_doEMfJetQualityCleaning  = true);
  declareProperty("doHECfJetQualityCleaning",    m_doHECfJetQualityCleaning  = true);
  declareProperty("doJetTimeCleaning",           m_doJetTimeCleaning  = true);
  declareProperty("doLowEMfCleaning",            m_doLowEMfCleaning  = false);
  // threshold for number of leading cells in (hecf > m_leadingCellsThr &&  nLeadingCells <= m_leadingCellsThr) cut:
  declareProperty("leadingCellsThr", m_leadingCellsThr = 5);
  // hecf threshold for cut in combination with nLeadingCells (i.e. hecf > m_hecfThrN &&  nLeadingCells <= m_leadingCellsThr):
  declareProperty("hecfThrN", m_hecfThrN = 0.8);
  // hecf threshold for cut in combination with jet Quality (i.e. hecf > m_hecfThrQ && |quality| > m_jetQualityThrH):
  declareProperty("hecfThrQ", m_hecfThrQ = 0.3);
  // quality threshold for cut in combination with hecf (i.e. hecf > m_hecfThrQ && |quality| > m_jetQualityThrH):
  declareProperty("jetQualityThrH", m_jetQualityThrH= 0.3);
  // quality threshold for cut in combination with emf (i.e. emf > m_emfThrQ && |quality| > m_jetQualityThrE):
  declareProperty("jetQualityThrE", m_jetQualityThrE = 0.6);
   // emf threshold for cut in combination with m_jetQualityThrE (i.e. emf > m_emfThrQ && quality > m_jetQualityThrE):
  declareProperty("emfThrQ", m_emfThrQ = 0.90);
  // emf threshold for Exclusive emf cut (i.e. emf < m_emfThrE):
  declareProperty("emfThrE", m_emfThrE = 0.01);
  // threshold for identification of out-of-time jets:
  declareProperty("jetTimeCellsThr", m_jetTimeCellsThr = 25.);

  declareMonitoredVariable("CutCounter", m_cutCounter);
  declareMonitoredVariable("logRatio",   m_logRatio);  
  declareMonitoredVariable("JetEt",      m_JetEt);
  // cleaning:
  declareMonitoredVariable("HECf",              m_hecf);
  declareMonitoredVariable("nLeadingCells",     m_nLeadingCells);
  declareMonitoredVariable("EMf",               m_emf);
  declareMonitoredVariable("fabs_jetQuality",   m_fabsjetQuality);
  declareMonitoredVariable("fabs_jetTimeCells", m_fabsjetTimeCells);
}
/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigL2HVJetHypo::~TrigL2HVJetHypo()
{ }

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2HVJetHypo::hltInitialize()
// ----------------------------------------------------------------------
{
  msg() << MSG::INFO << "in initialize()" << endmsg;

  m_accepted_L2=0;
  m_rejected_L2=0;
  m_errors_L2=0;


  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2HVJetHypo::hltFinalize(){
// ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events (Lvl2) accepted/rejected/errors:  "<< m_accepted_L2 <<" / "<<m_rejected_L2<< " / "<< m_errors_L2<< endmsg;
  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigL2HVJetHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
// ----------------------------------------------------------------------

  m_cutCounter = -1;

  //cleaning
  m_hecf = -99;
  m_nLeadingCells = -99;
  m_emf = -99;
  m_fabsjetQuality = -99;
  m_fabsjetTimeCells = -99;


  pass=false;


  HLT::ErrorCode status;

  // get the trigger element and extract the RoI information
  if(msgLvl() <= MSG::DEBUG) { // pure debug purposes...
    const TrigRoiDescriptor* roiDescriptor = 0;
    if ( getFeature(outputTE, roiDescriptor, "a_label")==HLT::OK ) {
      if ( roiDescriptor ) {
        if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: RoI id " << roiDescriptor->roiId()
            << " located at   phi = " <<  roiDescriptor->phi() << ", eta = " << roiDescriptor->eta() << endmsg;
      } else {
        if(msgLvl() <= MSG::DEBUG) msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endmsg;
      }
    } else {
      if(msgLvl() <= MSG::DEBUG) msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endmsg;
    }
  }

  //Timer for the L2 jet retrieval 

  std::vector<const TrigT2Jet*> vectorOfJets;

  HLT::ErrorCode ec = getFeatures(outputTE, vectorOfJets);

  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get the L2 Jets " << endmsg;
    m_errors_L2++; 
    return ec;
  }

  m_cutCounter = 0;

  // Check that there is only one JETROI.
  // We only expect ONE input RoI.
  if (vectorOfJets.size() != 1){
    msg() << MSG::ERROR << "The number of Jets attached to the TE is not 1" << endmsg;
    return HLT::OK;
  }

  // Get first (and only) RoI:
  const TrigT2Jet* pJet = vectorOfJets.back();
  if(!pJet){
    msg() << MSG::ERROR << "Retrieval of RoI from vector failed"  << endmsg;
    return HLT::OK;
  }



  //Timer for the computing of Log(H/E) and cuts on jets


  //double etjet = pJet->e();
  double etjet  = pJet->et();
  double ehjet  = pJet->ehad0();
  double emjet  = pJet->eem0();
  double jetEta = pJet->eta();
  double erat = -99999999.;
  double zero = 0.;

  if (CxxUtils::fpcompare::greater( ehjet, zero ) && CxxUtils::fpcompare::less_equal( emjet, zero )) erat = 999999;
  //if (emjet <= 0. && ehjet > 0.) erat = 999999;
  if (CxxUtils::fpcompare::greater( emjet, zero ) && CxxUtils::fpcompare::greater( ehjet, zero )) erat = log10(double(ehjet/emjet));
  //if (double(emjet) > 0. && ehjet > 0.) erat = log10(double(ehjet/emjet));

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jet energy (L2): " << etjet << endmsg;
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jet log10 of had / em energy ratio (L2): " << erat << endmsg;

  //monitored variables
  m_JetEt = etjet/1000;
  m_logRatio = erat; 

  //Cuts
  bool passCut = false;
  if ((etjet > m_EtCut_L2) && (fabs(jetEta) < m_JetEta) && (erat > m_LRatioCut_L2)) passCut = true;


  // jet cleaning:
  // -------------
  // global flag cleaningVeto: if this flag is set to true,
  // the hypo will reject the event (if in addition m_acceptAll == false)
  bool cleaningVeto = false;
 
  // n90 and hecfraction cut:
  if( m_doHECfnLeadingCellsCleaning == true && pJet->hecf() > m_hecfThrN && pJet->nLeadingCells() <= m_leadingCellsThr ){
   cleaningVeto = true;
  }
  // bad quality in EMC:
  if( m_doEMfJetQualityCleaning == true && pJet->emf() > m_emfThrQ && fabs(pJet->jetQuality())  > m_jetQualityThrE ){
    cleaningVeto = true;
  }
  // bad quality in HEC
  if( m_doHECfJetQualityCleaning == true && pJet->hecf() > m_hecfThrQ && fabs(pJet->jetQuality()) > m_jetQualityThrH ){
    cleaningVeto = true;
  }
  if( m_doHECfJetQualityCleaning == true && pJet->hecf() > (1-fabs(pJet->jetQuality())) ){
    cleaningVeto = true;
  }
  // out-of-time jets
  if( m_doJetTimeCleaning == true && fabs(pJet->jetTimeCells()) > m_jetTimeCellsThr ){
    cleaningVeto = true;
  } 
  // very small fraction of energy in EMC
  if(m_doLowEMfCleaning == true && pJet->emf() < m_emfThrE ){
    cleaningVeto = true;
  }




  if( (passCut && cleaningVeto == false) || m_acceptAll) {

    pass = true;  
    m_accepted_L2++; 
    m_cutCounter=1;

    // monitor cleaning:
    if(m_doHECfnLeadingCellsCleaning || m_doHECfJetQualityCleaning){
      m_hecf = pJet->hecf();
      m_nLeadingCells = pJet->nLeadingCells();
    }
    if(m_doEMfJetQualityCleaning || m_doLowEMfCleaning){
      m_emf = pJet->emf();
    }
    if(m_doHECfJetQualityCleaning || m_doEMfJetQualityCleaning){
      m_fabsjetQuality = fabs(pJet-> jetQuality());
    }
    if(m_doJetTimeCleaning){
      m_fabsjetTimeCells = fabs(pJet-> jetTimeCells());
    }

  } else {
    m_rejected_L2++;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Event rejected !" << endmsg;
  }


  return HLT::OK;

}
