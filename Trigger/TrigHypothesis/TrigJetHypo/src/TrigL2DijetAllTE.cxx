/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigL2DijetAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   Ignacio Aracena, Simon Owen, Tomasz Bold, Teresa Fonseca Martin, Josh McFayden
// ********************************************************************


#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/StatusCode.h"

//#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigJetHypo/TrigL2DijetAllTE.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigCaloEvent/TrigT2Jet.h"
#include "AthenaKernel/Units.h"

#include <math.h>
#include <algorithm>


#include "TrigInterfaces/AllTEAlgo.h"

//class ISvcLocator;
namespace Units = Athena::Units;

//Comparison function for pt sorting of JetToTEMap
bool bigger_by_pt ( const TrigL2DijetAllTE::JetToTEMap& a, const TrigL2DijetAllTE::JetToTEMap& b ) {
  return a.jet->pt() > b.jet->pt();
}



///////////////////////////////////////////////////////////////
// CONSTRUCTOR:
///////////////////////////////////////////////////////////////
TrigL2DijetAllTE::TrigL2DijetAllTE(const std::string & name, ISvcLocator* pSvcLocator)
 : HLT::AllTEAlgo(name, pSvcLocator)
{

  //Monitor
  declareProperty("doMonitoring_L2", m_doMonitoring = true, "switch on/off monitoring" );
  declareProperty("checkJetVectors", m_checkJetVectors = true, "check algorithm working as expected");
  declareMonitoredVariable("nInputTEsAllTE", m_nInputTEsAllTE);
  declareMonitoredVariable("nTEinTE0", m_nTEinTE0);
  declareMonitoredVariable("nTEinTE1", m_nTEinTE1);
  declareMonitoredVariable("ptHigh", m_ptHigh);
  declareMonitoredVariable("ptLow", m_ptLow);
  declareMonitoredVariable("nLowChainHigh", m_nLowChainHigh);
  declareMonitoredVariable("ptHighChain_0", m_ptHighChain_0);
  declareMonitoredVariable("ptLowChain_0", m_ptLowChain_0);
  declareMonitoredVariable("ptLowChain_1", m_ptLowChain_1);
}

HLT::ErrorCode TrigL2DijetAllTE::hltInitialize()
{
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Initialization. The correct configuration of this algorithm "
     << "requires jets ordered in increasing energy" << endmsg;

  return HLT::OK;
}

///////////////////////////////////////////////////////////////
// DESTRUCTOR:
///////////////////////////////////////////////////////////////
TrigL2DijetAllTE::~TrigL2DijetAllTE(){
}

HLT::ErrorCode TrigL2DijetAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
				     unsigned int type_out)
{

  beforeExecMonitors().ignore();

  //Monitoring variables
  m_nInputTEsAllTE = -1;
  m_nTEinTE0 = -1;
  m_nTEinTE1 = -1;
  m_ptHigh = -999.9;
  m_ptLow = -999.9;
  m_nLowChainHigh = -1;
  m_ptHighChain_0 = -999.9;
  m_ptLowChain_0 = -999.9;
  m_ptLowChain_1 = -999.9;
  
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Executing this TrigL2DijetAllTE" << name() << endmsg;
  }
  
  m_nInputTEsAllTE = tes_in.size();
  if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "Number of input TE's = " << m_nInputTEsAllTE << endmsg;

  // There should be two input TEs (e.g. for L2_2j15_j60 there is j15 TE and j60 TE)
  if ( m_nInputTEsAllTE != 2 ) {
    msg() << MSG::WARNING << "Got different than 2 number of input TEs" << endmsg;
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE); 
  }
    
  //Sanity configuration checks
  m_nTEinTE0 = tes_in[0].size();
  if (m_nTEinTE0 < 1) {
    if (msgLvl()<=MSG::WARNING) msg() << MSG::WARNING << "There are " <<  m_nTEinTE0 << " input TE for tes_in[0]" << endmsg;

    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  }

  m_nTEinTE1 = tes_in[1].size();
  if (m_nTEinTE1 < 1) {
    if (msgLvl()<=MSG::WARNING) msg() << MSG::WARNING << "There are " <<  m_nTEinTE1 << " input TE for tes_in[1]" << endmsg;

    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  }

  // Important: This algorithm requires to be configured having as input Jet TE in increasing order of energy, otherwise it doesn't work, that's the reason to have an ERROR here:
  if ( m_nTEinTE0 < m_nTEinTE1 ) {
    if (msgLvl()<=MSG::WARNING) {
      msg() << MSG::WARNING << "REGTEST: BAD error. Trigger chain probably not working at all" << endmsg;
      msg() << MSG::WARNING << "REGTEST: Probably chain badly configured. Input TE chains should be given ordered in increasing energy" << endmsg;
      msg() << MSG::WARNING << "REGTEST: First vector of TE size: " << m_nTEinTE0 << " should be the same or bigger than second: " << m_nTEinTE1 << endmsg;
    }
   
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
    
  }
  

  std::vector<JetToTEMap> jetmap_low;
  std::vector<JetToTEMap> jetmap_high;
  //store association between L2 jet object and its corresponding TE
  pull_jets(tes_in[0], jetmap_low);
  pull_jets(tes_in[1], jetmap_high);
  
  // Come back: do i want to check again that jetmap size >= 1?
  if (jetmap_high.size() < 1 || jetmap_low.size() < 2) {
    if (msgLvl()<=MSG::WARNING)  msg() << MSG::WARNING << "REGTEST: less jets than expected. jetmap_high.size(): " 
      <<  jetmap_high.size() << " low: " << jetmap_low.size() << endmsg;
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  }

  //sort it according to jet energy (highest jet energy in first vector element)
  sort_jets(jetmap_low);
  sort_jets(jetmap_high);

  //check to be activated through jO to check that algorithm is behaving as expected
  //can be deactivated to save time when trigger is well commisioned
  if (m_checkJetVectors) check(jetmap_high, jetmap_low);
  
  //store the correct jet TEs into the output TE. 
  HLT::TEVec seed_for_next;
  store_jets(seed_for_next, jetmap_high, jetmap_low);  
  
  if ( seed_for_next.size() == 2 ){
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(seed_for_next, type_out);
    outputTE->setActiveState(true);
    if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: TE created" << endmsg;
  } else {
    if (msgLvl()<=MSG::WARNING)  msg() << MSG::WARNING << "REGTEST: number of jets: " << seed_for_next.size()
                                     << " instead of 2, no TE created" << endmsg;
    afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  }
  afterExecMonitors().ignore();
  return HLT::OK;
}

//Function to store correspondency between a jet object and corresponding TE
HLT::ErrorCode TrigL2DijetAllTE::pull_jets( HLT::TEVec& tes,  std::vector<JetToTEMap>& jm) {
  for ( HLT::TEVec::iterator it = tes.begin(); it != tes.end(); ++it) {
    const TrigT2Jet* jet(0); // not expecting more than one Jet here !!!
    if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG  << "TE iterator count = " << it - tes.begin() << endmsg;

    //HACK to make getFeature function work...
    const HLT::TriggerElement* tempTE = (*it);
        
    if (HLT::Algo::getFeature(tempTE,jet) != HLT::OK || jet == 0 ) {
      msg() <<  MSG::WARNING << "No RoIDescriptors for this Trigger Element! " << endmsg;
      
    } else {
      if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG  << "Looking at ROI descriptor" << endmsg;
      jm.push_back(JetToTEMap(jet, *it));
      if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG  << "Map filled with jet&TE. Jet pt: " << jet->pt()   
                                      << " eta: " << jet->eta() << " phi: " << jet->phi()
                                      << " et: " << jet->et()
                                      << endmsg;
    }
    
    const TrigRoiDescriptor* roiDescriptor = 0;
    //if ( getFeature(outputTE, roiDescriptor, "a_label")==HLT::OK ) {
    if ( getFeature(tempTE, roiDescriptor, "initialRoI")==HLT::OK ) {
      if ( roiDescriptor ) {
        msg() << MSG::DEBUG << "REGTEST: RoI id " << roiDescriptor->roiId()
              << " located at phi = " <<  roiDescriptor->phi() << ", eta = " << roiDescriptor->eta() << endmsg;
      } else {
        msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endmsg;
      } } else {
      msg() <<  MSG::DEBUG << "Failed to find RoiDescriptor " << endmsg;
    }
    
  }
  return HLT::OK;
}

//Function to sort jets in pt decreasing order
void TrigL2DijetAllTE::sort_jets( std::vector<JetToTEMap>& jm) {
  std::sort(jm.begin(), jm.end(), bigger_by_pt);
}

//Function to check that all High Pt TE_Jets are contained in the set of lower Pt TE_jets
HLT::ErrorCode TrigL2DijetAllTE::check( const std::vector<JetToTEMap>& jHigh, const std::vector<JetToTEMap>& jLow) {
  if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG  << "REGTEST: Number of jets passing higher pt threshold: " 
                                  << jHigh.size() << " and low: " << jLow.size() << endmsg;
  for (unsigned int in=0; in < jHigh.size(); ++in) {
    bool checkJet = false;
    for (unsigned int inlow=0; inlow < jLow.size(); ++inlow) {
      if (jHigh[in].jet == jLow[inlow].jet) checkJet = true;
    }
    if (checkJet == false) {
      if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG 
                                        << "REGTEST: Extremely dangerous error THIS ALGORITHM IS NOT WORKING PROPERLY"
                                        << " Jet TE with high pt threshold not contained in set of Jet TE with lower pt threshold " 
                                        << " For Jet: " << in << " Pt high: " << jHigh[in].jet->pt() << endmsg;
    }
    
    //Print out details of jets in high pt and low pt chains.
    if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG
                                    << "Details of high pt chain: Jet: " << in << " Pt: " << jHigh[in].jet->pt()
                                    << " eta: " << jHigh[in].jet->eta() << " phi: " << jHigh[in].jet->phi()
                                    << " et: " << jHigh[in].jet->et() 
                                    << endmsg;
  }
  for (unsigned int in=0; in < jLow.size(); ++in) {
    if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG
                                    << "Details of low pt chain: Jet: " << in << " Pt: " << jLow[in].jet->pt()
                                    << " eta: " << jLow[in].jet->eta() << " phi: " << jLow[in].jet->phi()
                                    << " et: " << jLow[in].jet->et() 
                                    << endmsg;
  }
  
  return HLT::OK;
}

//Function to store the correct TEs in the output TE
HLT::ErrorCode TrigL2DijetAllTE::store_jets(HLT::TEVec& outputTE,  const std::vector<JetToTEMap>& jmHigh, const std::vector<JetToTEMap>& jmLow) {
  
  //Monitoring
  m_ptHighChain_0 = jmHigh[0].jet->pt()/Units::GeV;
  m_ptLowChain_0 = jmLow[0].jet->pt()/Units::GeV;
  m_ptLowChain_1 = jmLow[1].jet->pt()/Units::GeV;
  
  //Remember we need at least one jet from high pt chain
  // Low chain can have a higher pt jet than in higher chain if there is poor L1 reco. This jet will exist at jmLow[0]
  if (jmLow[0].jet != jmHigh[0].jet) {
    outputTE.push_back(jmLow[0].te);
    outputTE.push_back(jmHigh[0].te);
    m_ptHigh = jmLow[0].jet->pt()/Units::GeV;
    m_ptLow = jmHigh[0].jet->pt()/Units::GeV;
    m_nLowChainHigh = 1;
  }  
  else {
    outputTE.push_back(jmHigh[0].te);
    outputTE.push_back(jmLow[1].te);
    m_ptHigh = jmHigh[0].jet->pt()/Units::GeV;
    m_ptLow = jmLow[1].jet->pt()/Units::GeV;
    m_nLowChainHigh = 0;
  }
  if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "REGTEST: Stored 2 jets with pts: " << m_ptHigh
                                  << " GeV, " << m_ptLow << " GeV " << endmsg;
  return HLT::OK;
}



HLT::ErrorCode TrigL2DijetAllTE::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endmsg;

  return HLT::OK;
}

