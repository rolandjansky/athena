/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2JetMctHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo/
//
// AUTHOR:   Simon Owen, Josh McFayden
// ********************************************************************


#include "GaudiKernel/IToolSvc.h"
// Not sure ...
//#include "GaudiKernel/StatusCode.h"
//#include "TrigT1Interfaces/RecEmTauRoI.h"

#include "TrigInterfaces/ComboAlgo.h"
#include "TrigJetHypo/TrigL2JetMctHypo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "AthenaKernel/Units.h"

#include <math.h>

class ISvcLocator;
namespace Units = Athena::Units;

TrigL2JetMctHypo::TrigL2JetMctHypo(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::HypoAlgo(name, pSvcLocator),
    m_vjetet1(0),
    m_vjeteta1(0),
    m_vjetphi1(0),
    m_vjetet2(0),
    m_vjeteta2(0),
    m_vjetphi2(0)
{

  declareProperty("JetMctCut", m_JetMctCut = 150*Units::GeV, "Mct cut on the L2 jets ");
  declareProperty("ptHardJetCut", m_ptHardJetCut = 90*Units::GeV, "Pt threshold for the hardest L2 jet");
  declareProperty("ptSoftJetCut", m_ptSoftJetCut =  90*Units::GeV, "Pt threshold for the 2nd hardest L2 jet");

  //Monitor
  declareProperty("doMonitoring_L2", m_doMonitoring = true, "switch on/off monitoring" );
  declareMonitoredVariable("nTeMctHypoCounter", m_nTeMctHypoCounter);
  declareMonitoredVariable("nJetsTe1", m_nJetsTe1);
  declareMonitoredVariable("nJetsTe2", m_nJetsTe2);
  declareMonitoredVariable("mct", m_mct);
  declareMonitoredVariable("Passed_mct", m_Passed_mct);
  declareMonitoredVariable("dPhi", m_dphi);
  declareMonitoredVariable("ptLeadingJet", m_ptLeadingJet);
  declareMonitoredVariable("etaLeadingJet", m_etaLeadingJet);
  declareMonitoredVariable("phiLeadingJet", m_phiLeadingJet);
  declareMonitoredVariable("ptNextLeadingJet", m_ptNextLeadingJet);
  declareMonitoredVariable("etaNextLeadingJet", m_etaNextLeadingJet);
  declareMonitoredVariable("phiNextLeadingJet", m_phiNextLeadingJet);
  declareMonitoredVariable("MctHypoResult", m_MctHypoResult);
}

HLT::ErrorCode TrigL2JetMctHypo::hltInitialize()
{
  // No timing internal to algorithm. Should be done automatically.
  /*
  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    msg() << MSG::WARNING << name()
	  << ": Unable to locate TrigTimer Service" << endmsg;
  }
  if (m_timersvc) {
    
    TrigTimer* tmp = m_timersvc->addItem("TrigL2JetMctHypo.TrigL2JetMctHypoTot");
    m_timers.push_back(tmp);
  }
  */

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization:" << endmsg;
    msg() << MSG::DEBUG << "REGTEST Cut values: JetMctCut: " << m_JetMctCut 
	  << ", ptHardJetCut: " << m_ptHardJetCut 
	  << ", ptSoftJetCut: " << m_ptSoftJetCut << endmsg;
  }

  return HLT::OK;
}

TrigL2JetMctHypo::~TrigL2JetMctHypo(){
}

//HLT::ErrorCode TrigL2JetMctHypo::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
//    unsigned int type_out)
HLT::ErrorCode TrigL2JetMctHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
  // Time total TrigL2JetMctHypo execution time.
  //if (m_timersvc) m_timers[0]->start();

  // by default the trigger is not satisfied
  pass = false;
  
  //Monitoring variables
  m_MctHypoResult = 0;
  m_nTeMctHypoCounter = -1;
  m_nJetsTe1 = -1;
  m_nJetsTe2 = -1;
  m_ptLeadingJet = -1.0;
  m_etaLeadingJet = -999.9;
  m_phiLeadingJet = -999.9;
  m_ptNextLeadingJet = -1.0;
  m_etaNextLeadingJet = -999.9;
  m_phiNextLeadingJet = -999.9;
  m_mct = -999.9;
  m_dphi = -999.9;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Executing this TrigL2JetMctHypo: " << name() << endmsg;
  }
  
  // this are 2 TEs which we eventually will combine
  m_nTeMctHypoCounter = HLT::Navigation::getDirectPredecessors(outputTE).size();
  if ( m_nTeMctHypoCounter != 2 ) {
    msg() << MSG::WARNING << "Got diferent than 2 number of input TEs: " << m_nTeMctHypoCounter  << " job badly configured" << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);  
  }
  
  const HLT::TriggerElement* te1 = HLT::Navigation::getDirectPredecessors(outputTE).at(0);
  const HLT::TriggerElement* te2 = HLT::Navigation::getDirectPredecessors(outputTE).at(1);


  //Retrieve L2 jets associated to TEs
  std::vector<const TrigT2Jet*> vectorOfJets1;
  std::vector<const TrigT2Jet*> vectorOfJets2;

  if ( getFeatures(te1, vectorOfJets1)  != HLT::OK || getFeatures(te2,vectorOfJets2) != HLT::OK ||
       vectorOfJets1.at(0) == 0 || vectorOfJets2.at(0)  == 0 ) {
    if ( msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigT2Jet vector" << endmsg;
    }
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  }

  m_nJetsTe1 = vectorOfJets1.size();
  m_nJetsTe2 = vectorOfJets2.size();
  
  if (msgLvl()<=MSG::DEBUG) {
    msg() << MSG::DEBUG << " Got " << m_nJetsTe1 << " Jets from TE1" << endmsg;
    msg() << MSG::DEBUG << " Got " << m_nJetsTe2 << " Jets from TE2" << endmsg;
  }
  

  //Not sure sanity check
  /*
  const TrigRoiDescriptor* roiDescriptor = 0;
  if (getFeature(te1,roiDescriptor) != HLT::OK)   msg() <<  MSG::WARNING << "No RoIDescriptors for this Trigger Element 1! " << endmsg;
  else if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG  << "Looking at ROI descriptors for TE1" << endmsg;
  roiDescriptor = 0;
  if (getFeature(te2,roiDescriptor) != HLT::OK)   msg() <<  MSG::WARNING << "No RoIDescriptors for this Trigger Element 2! " << endmsg;
  else if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG  << "Looking at ROI descriptors for TE2" << endmsg;
  */
  
  //Not sure ... If there's only 1 jet per TE do we really need to do the ordering (below)? Leave it in for now..  

  // Loop over L2 jets for TE1
  // order them in decreasing energy and store them in vector
  m_vjetet1.clear();
  m_vjeteta1.clear();
  m_vjetphi1.clear();
  for (std::vector<const TrigT2Jet*>::const_iterator vIt=vectorOfJets1.begin(); vIt!=vectorOfJets1.end(); ++vIt) {
    float jetet1 = (*vIt)->et();
    float jeteta1 = (*vIt)->eta();
    float jetphi1 = (*vIt)->phi();
    int vjetn1 = m_vjetet1.size();
    if(vjetn1==0){
      m_vjetet1.push_back(jetet1);
      m_vjeteta1.push_back(jeteta1);
      m_vjetphi1.push_back(jetphi1);
    } 
    else {
      for(int j=0;j!=vjetn1;j++){
        if((jetet1 > m_vjetet1[j])) {	  
	  m_vjetet1.insert(m_vjetet1.begin()+j,1,jetet1);
	  m_vjeteta1.insert(m_vjeteta1.begin()+j,1,jeteta1);
	  m_vjetphi1.insert(m_vjetphi1.begin()+j,1,jetphi1);
	  break;
	} else if(j==vjetn1-1){
	  m_vjetet1.push_back(jetet1);
	  m_vjeteta1.push_back(jeteta1);
	  m_vjetphi1.push_back(jetphi1);
	  break;
        }
      }
    }
  } // end loop over jets for TE1

  // Loop over L2 jets for TE2
  // order them in decreasing energy and store them in vector
  m_vjetet2.clear();
  m_vjeteta2.clear();
  m_vjetphi2.clear();
  for(std::vector<const TrigT2Jet*>::const_iterator vIt=vectorOfJets2.begin();
      vIt!=vectorOfJets2.end(); ++vIt){
    float jetet2 = (*vIt)->et();
    float jeteta2 = (*vIt)->eta();
    float jetphi2 = (*vIt)->phi();
    int vjetn2 = m_vjetet2.size();
    if(vjetn2==0){
      m_vjetet2.push_back(jetet2);
      m_vjeteta2.push_back(jeteta2);
      m_vjetphi2.push_back(jetphi2);
    } else {
      for(int j=0;j!=vjetn2;j++){
	if((jetet2 > m_vjetet2[j])){
	  m_vjetet2.insert(m_vjetet2.begin()+j,1,jetet2);
	  m_vjeteta2.insert(m_vjeteta2.begin()+j,1,jeteta2);
	  m_vjetphi2.insert(m_vjetphi2.begin()+j,1,jetphi2);
	  break;
	} else if(j==vjetn2-1){
	  m_vjetet2.push_back(jetet2);
	  m_vjeteta2.push_back(jeteta2);
	  m_vjetphi2.push_back(jetphi2);
	  break;
	}
      }
    }
  } // end loop over jets

  // Note that we are comparing only highest pt jet in the Roi (as opposed to everybody to everybody)

  //Compute dphi, get its absolute value, make sure it is < pi
  m_dphi = m_vjetphi1.at(0) - m_vjetphi2.at(0);
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "REGTEST Phi1: " << m_vjetphi1.at(0) 
          << " Phi2: " << m_vjetphi2.at(0) << " Dphi: " << m_dphi << endmsg;
    //msg() << MSG::DEBUG << "REGTEST Et1: " << m_vjetet1.at(0) << " Et2: " << m_vjetet2.at(0) << endmsg;
  }
  
  //check that you are computing dphi 'properly'
  
  if (fabs(m_dphi) > M_PI ) m_dphi = 2*M_PI - fabs(m_dphi);
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "After correction (to get equivalent angle below PI) Dphi: " << m_dphi << endmsg;
  m_dphi = fabs(m_dphi);
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Absolute value of Dphi: " << m_dphi << endmsg;

  m_mct = sqrt(2*m_vjetet1.at(0)*m_vjetet2.at(0)*(1+cos(m_dphi))) / Units::GeV;
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Value of Mct: " << m_mct*Units::GeV << endmsg;
  
  // Perform selection
  if (m_vjetet1.at(0) >= m_vjetet2.at(0) ) {
    //Monitoring
    m_ptLeadingJet = m_vjetet1.at(0)/Units::GeV;
    m_etaLeadingJet = m_vjeteta1.at(0);
    m_phiLeadingJet = m_vjetphi1.at(0);
    m_ptNextLeadingJet = m_vjetet2.at(0)/Units::GeV;
    m_etaNextLeadingJet = m_vjeteta2.at(0);
    m_phiNextLeadingJet = m_vjetphi2.at(0);
    
  } else {
    
    //Monitoring
    m_ptLeadingJet = m_vjetet2.at(0)/Units::GeV;
    m_etaLeadingJet = m_vjeteta2.at(0);
    m_phiLeadingJet = m_vjetphi2.at(0);
    m_ptNextLeadingJet = m_vjetet1.at(0)/Units::GeV;
    m_etaNextLeadingJet = m_vjeteta1.at(0);
    m_phiNextLeadingJet = m_vjetphi1.at(0); 
  }
  
  // Et cut on hardest jet
  if (m_ptLeadingJet*Units::GeV < m_ptHardJetCut) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Hardest L2 Jet: " << m_ptLeadingJet*Units::GeV << " failed Et cut: " << m_ptHardJetCut << endmsg;
    return HLT::OK;
  }
  // Et cut on second hardest jet
  if (m_ptNextLeadingJet*Units::GeV < m_ptSoftJetCut) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Second hardest L2 Jet: " << m_ptNextLeadingJet*Units::GeV << " failed Et cut: " << m_ptSoftJetCut << endmsg;
    return HLT::OK;
  }
  // Mct cut
  if (m_mct*Units::GeV < m_JetMctCut) {
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Mct: " << m_mct*Units::GeV << " failed Mct cut: " << m_JetMctCut << endmsg;
    return HLT::OK;
  }
  // trigger satisfied, let the steering know
  pass = true;
  m_MctHypoResult = 1;
  m_Passed_mct = m_mct;
  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST pass = " << pass << " Mct: " << m_mct*Units::GeV 
                                   << " Phi1: " << m_phiLeadingJet << " Phi2: " << m_phiNextLeadingJet
                                   << " Et hardest jet: " << m_ptLeadingJet*Units::GeV << " Et 2nd hardest jet: " << m_ptNextLeadingJet*Units::GeV  
                                   << endmsg;
  
  return HLT::OK;
}


HLT::ErrorCode TrigL2JetMctHypo::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;

  return HLT::OK;
}
