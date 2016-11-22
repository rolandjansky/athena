/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFJetMassY.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   Oliver Endner
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

#include "TrigJetHypo/TrigEFJetMassY.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
#include "FourMomUtils/P4DescendingSorters.h"
#include "JetUtils/JetCaloQualityUtils.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigPassBits.h"



class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFJetMassY::TrigEFJetMassY(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("Masscut",   m_MjjCut = 2000*CLHEP::GeV); // Default: 2000 GeV
  declareProperty("Ystarcut",   m_YstarCut = 1.7); // Default: 1.7
  declareProperty("doMonitoring", m_doMonitoring = false );
  declareProperty("AcceptAll",      m_acceptAll=false);

  declareMonitoredVariable("CutCounter", m_cutCounter);

  // Monitored variables...
  declareMonitoredVariable("NJet", m_jetnr);
  declareMonitoredVariable("Ystar", m_ystar);
  declareMonitoredVariable("Mjj", m_mjj);
  declareMonitoredVariable("Mjj_cuted", m_mjj_cuted);
  declareMonitoredVariable("Ystar_cuted", m_ystar_cuted);
  declareMonitoredVariable("pt", m_pt);
  declareMonitoredVariable("pt_cut", m_pt_cuted);
  declareMonitoredVariable("eta", m_eta);
  declareMonitoredVariable("eta_cut", m_eta_cuted);
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFJetMassY::~TrigEFJetMassY()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetMassY::hltInitialize()
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

    TrigTimer* tmp = m_timersvc->addItem("TrigEFJetMassY.TrigEFJetMassYTot");
    m_timers.push_back(tmp);
  }

  m_accepted=0;
  m_rejected=0;
  m_errors=0;
  return HLT::OK;

}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetMassY::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endmsg;
  return HLT::OK;

}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetMassY::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  // ----------------------------------------------------------------------

  m_cutCounter = -1;

  pass=false;

  m_jetnr = -99.0;
  m_ystar = -99.;
  m_mjj = -99.;
  m_ystar_cuted = -99.;
  m_mjj_cuted = -99.;
  m_eta = -99.;
  m_eta_cuted = -99.;
  m_pt = -99.;
  m_pt_cuted = -99.;
  


  const JetCollection* outJets(0);
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

  std::vector<const Jet*> theJets(outJets->begin(), outJets->end());


 //check size of JetCollection
  if( theJets.size() == 0 ){
    msg()<< MSG::DEBUG << " Size of JetCollection is 0! " << endmsg;
    m_errors++;
    if (m_acceptAll) m_cutCounter = 1;
    return HLT::OK;
  }

  std::sort (theJets.begin(), theJets.end(), P4Sorters::Descending::Pt());


  ///  now add the TrigPassBits for the jets to specify whether each jet
  ///  passes the hypo etc

  TrigPassBits* bits = HLT::makeTrigPassBits( outJets );


//  unsigned int jetnr =0;
  m_jetnr=0;

  const Jet *aJet2 = 0;
  for (const Jet* aJet : theJets) {
    m_jetnr++;
    if(m_jetnr==1){
      aJet2 = aJet;
      // if there is just one jet count the event as rejected
      m_rejected++;
    }
    if(m_jetnr==2){
      m_ystar = 0.5*fabs(((aJet)->hlv()).rapidity() - ((aJet2)->hlv()).rapidity());
      m_mjj=((aJet)->hlv() + (aJet2)->hlv()).m();
      m_eta = ((aJet2)->hlv()).eta();
      m_pt = sqrt(pow(((aJet2)->hlv()).px(),2)+pow(((aJet2)->hlv()).py(),2));
      if(m_mjj>m_MjjCut&&m_ystar<m_YstarCut){
        HLT::markPassing( bits, aJet2, outJets );
        m_accepted++;
        m_rejected--;
        pass=true;
        m_mjj_cuted=m_mjj;
        m_ystar_cuted=m_ystar;
        m_eta_cuted=m_eta;
        m_pt_cuted=m_pt;
      }
      // If the event passes the cuts, mark all Jets as having passed
      if(pass){
        HLT::markPassing( bits, aJet, outJets );
      }		
    }
  }


  // attach the bits
  if ( attachBits(outputTE, bits ) != HLT::OK ) {
    msg() << MSG::ERROR << "Problem attaching TrigPassBits for the Jets " << endmsg;
  }


  return HLT::OK;

}
