/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFJetMassDEta.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   Justin Griffiths
//    clone of TrigEFJetMassY
//    cut on DEta rather than YStar
//    Consider all Jet pairs with at least XX Pt
//
// ********************************************************************
//
#include <algorithm>
#include <list>
#include <iterator>
#include <sstream>
#include <cmath>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/Enums.h"

#include "TrigJetHypo/TrigEFJetMassDEta.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "FourMomUtils/P4DescendingSorters.h"
#include "JetUtils/JetCaloQualityUtils.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigPassBits.h"



class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFJetMassDEta::TrigEFJetMassDEta(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("MjjCut",       m_MjjCut = 200*CLHEP::GeV); // Default: 200 GeV
  declareProperty("DEtaCut",      m_dEtaCut = 2.0); // Default: 2.0
  declareProperty("doMonitoring", m_doMonitoring = false );
  declareProperty("AcceptAll",    m_acceptAll=false);
  declareProperty("EtCut", m_EtCut=0*CLHEP::GeV);//leave default value 0 for backwards compatibility
  declareProperty("EtaMaxCut", m_etaMaxCut=100);

  declareMonitoredVariable("CutCounter", m_cutCounter);

  // Monitored variables...
  declareMonitoredVariable("NJet", m_jetnr);
  declareMonitoredVariable("Complexity", m_complexity);
  declareMonitoredVariable("dEta", m_deta);
  declareMonitoredVariable("Mjj", m_mjj);
  declareMonitoredVariable("Mjj_cuted", m_mjj_cuted);
  declareMonitoredVariable("dEta_cuted", m_deta_cuted);

  declareMonitoredStdContainer("pt", m_pt);
  declareMonitoredStdContainer("pt_cut", m_pt_cuted);
  declareMonitoredStdContainer("eta", m_eta);
  declareMonitoredStdContainer("eta_cut", m_eta_cuted);
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFJetMassDEta::~TrigEFJetMassDEta()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetMassDEta::hltInitialize()
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

    TrigTimer* tmp = m_timersvc->addItem("TrigEFJetMassDEta.TrigEFJetMassDEtaTot");
    m_timers.push_back(tmp);
  }

  m_accepted=0;
  m_rejected=0;
  m_errors=0;
  return HLT::OK;

}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetMassDEta::hltFinalize(){
  // ----------------------------------------------------------------------

  m_pt.clear();
  m_pt_cuted.clear();
  m_eta.clear();
  m_eta_cuted.clear();

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endmsg;
  return HLT::OK;

}

struct DescendingEta:std::binary_function<const xAOD::Jet*,
                                         const xAOD::Jet*,
                                         bool> {
  bool operator()(const xAOD::Jet* l, const xAOD::Jet* r)  const {
    return fabs(l->eta()) > fabs(r->eta());
  }
};

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetMassDEta::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  // ----------------------------------------------------------------------

  msg() << MSG::DEBUG << "Hello from " << name() << endmsg;

  m_pt.clear();
  m_pt_cuted.clear();
  m_eta.clear();
  m_eta_cuted.clear();

  m_cutCounter = -1;

  pass=false;

  m_jetnr = -99.0;
  m_deta = -99.;
  m_mjj = -99.;
  m_deta_cuted = -99.;
  m_mjj_cuted = -99.;
  


  const xAOD::JetContainer* outJets(0);
  HLT::ErrorCode ec = getFeature(outputTE, outJets);

  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get JetCollections " << endmsg;
    return ec;
  }


  m_cutCounter = 0;
  if( outJets == 0 ){
    msg() << MSG::DEBUG << " Got no JetCollections associated to the TE! " << endmsg;
    m_errors++;
    if (m_acceptAll) m_cutCounter = 1;
    return HLT::OK;
  }

  std::vector<const xAOD::Jet*> theJets(outJets->begin(), outJets->end());
  std::vector<const xAOD::Jet*> theSelectedJets;

 //check size of JetCollection
  if( theJets.size() == 0 ){
    msg()<< MSG::DEBUG << " Size of JetCollection is 0! " << endmsg;
    m_errors++;
    if (m_acceptAll) m_cutCounter = 1;
    m_rejected++;
    m_rejected++;
    return HLT::OK;
  }

  theSelectedJets.reserve (theJets.size()); 
  for (const xAOD::Jet* j : theJets) {
    if(j->p4().Et() < m_EtCut) continue;
    if(fabs(j->eta()) > m_etaMaxCut) continue;
    theSelectedJets.push_back(j);
    //monitoring
    m_pt.push_back(j->p4().Et());
    m_eta.push_back(j->eta());
  }

 //check size of JetCollection
  if( theSelectedJets.size() < 2 ){
    msg()<< MSG::DEBUG << " Size of Selected Jets JetCollection is < 2 " << endmsg;
    //    m_errors++;
    if (m_acceptAll) m_cutCounter = 1;
    m_rejected++;
    return HLT::OK;
  }
 
  std::sort (theSelectedJets.begin(), theSelectedJets.end(), DescendingEta());

  ///  now add the TrigPassBits for the jets to specify whether each jet
  ///  passes the hypo etc
  TrigPassBits* bits = HLT::makeTrigPassBits( outJets );

  // only one jet per JetCollection (for the moment, change it in the future)
  auto firstJet = theSelectedJets.begin();
  auto lastJet  = theSelectedJets.end();

//  unsigned int jetnr =0;
  m_jetnr=theSelectedJets.size();

  bool clean_break(0);
  for(; firstJet!=lastJet && !clean_break; firstJet++) {
    const xAOD::Jet *aJet = *firstJet;
    for(auto nthJet = theSelectedJets.rbegin();
	(*nthJet) != (*firstJet); ++nthJet){
      const xAOD::Jet *aJet2 = *nthJet;

      float deta = fabs(aJet->eta() - aJet2->eta());
      //monitoring
      m_complexity++;
      if(deta > m_deta) m_deta = deta;
      //
      if(deta < m_dEtaCut) {
	//attempt to speed up algorithm since jets are eta sorted
	if(nthJet==theSelectedJets.rbegin()) clean_break = true;
	break;
      }
      if(deta > m_deta) m_deta = deta;
      
      float mjj=(aJet->p4() + aJet2->p4()).M(); 
      //monitoring
      if(mjj > m_mjj) m_mjj = mjj;
      if(mjj>m_MjjCut&&deta>m_dEtaCut){	
        HLT::markPassing( bits, aJet, outJets );
        HLT::markPassing( bits, aJet2, outJets );
        pass=true;
	//monitoring
        if(mjj>m_mjj_cuted) {
	  m_mjj_cuted=mjj;
	  m_deta_cuted=deta;
	}
	m_eta_cuted.push_back(aJet->eta());
	m_eta_cuted.push_back(aJet2->eta());
	m_pt_cuted.push_back(aJet->pt());
	m_pt_cuted.push_back(aJet2->pt());
      }
    }
  }

  if(!pass) m_rejected++;
  else m_accepted++;

  // attach the bits
  if ( attachBits(outputTE, bits ) != HLT::OK ) {
    msg() << MSG::ERROR << "Problem attaching TrigPassBits for the Jets " << endmsg;
  }


  return HLT::OK;

}


