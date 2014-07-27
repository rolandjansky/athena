/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFCentralJet.cxx (based on TrigEFJetHypo)
//
// AUTHOR:
//
// ********************************************************************
//
#include <list>
#include <iterator>
#include <sstream>
#include <cmath>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

//#include "TrigConfHLTData/HLTTriggerElement.h"

//#include "TrigSteeringEvent/TriggerElement.h"
//#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigJetHypo/TrigEFCentralJet.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
#include "FourMomUtils/P4DescendingSorters.h"
#include "CLHEP/Units/SystemOfUnits.h"



class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFCentralJet::TrigEFCentralJet(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("eta_range",   eta_range = 1.1 ); // Default: 1.1
  declareProperty("doMonitoring", m_doMonitoring = false );


  // Monitored variables...

  declareMonitoredVariable("all_jet0_et", all_jet0_et );
  declareMonitoredVariable("all_jet0_eta",all_jet0_eta );
  declareMonitoredVariable("passed_jet0_et", passed_jet0_et );
  declareMonitoredVariable("passed_jet0_eta",passed_jet0_eta );
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFCentralJet::~TrigEFCentralJet()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFCentralJet::hltInitialize()
  // ----------------------------------------------------------------------
{
  msg() << MSG::INFO << "in initialize()" << endreq;

  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    msg() << MSG::WARNING << name()
	  << ": Unable to locate TrigTimer Service" << endreq;
  }
  if (m_timersvc) {

    TrigTimer* tmp = m_timersvc->addItem("TrigEFCentralJet.TrigEFCentralJetTot");
    m_timers.push_back(tmp);
  }

  m_accepted=0;
  m_rejected=0;
  m_errors=0;

  return HLT::OK;

}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFCentralJet::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endreq;
  return HLT::OK;

}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFCentralJet::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  // ----------------------------------------------------------------------


  // Time total TrigEFCentralJet execution time.
  // -------------------------------------
  //  if (m_timersvc) m_timers[0]->start();

  if(msgLvl() <= MSG::DEBUG) {
    //    std::string label;
    //    TrigConf::HLTTriggerElement::getLabel (TEout->getId(), label );
    //    msg() << MSG::DEBUG <<"REGTEST:"<< name() << ": in hltExecute() on: " << label << endreq;
  }
  pass=false;

  const JetCollection* outJets(0);
  HLT::ErrorCode ec = getFeature(outputTE, outJets);

  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get JetCollections " << endreq;
    return ec;
  }

  if( outJets == 0 ){
    msg() << MSG::DEBUG << " Got no JetCollections associated to the TE! " << endreq;
    m_errors++;
    return HLT::OK;
  }

  // JetCollection -> jetcolletcion_t
  std::vector<const Jet*> theJets(outJets->begin(), outJets->end());

  //check size of JetCollection
  if( theJets.size() == 0 ){
    msg()<< MSG::DEBUG << " Size of JetCollection is 0! " << endreq;
    m_errors++;
    //    if (m_timersvc) m_timers[0]->stop();
    return HLT::OK;
  }
  std::sort (theJets.begin(), theJets.end(), P4Sorters::Descending::Et());

  // only one jet per JetCollection (for the moment, change it in the future)
  
  unsigned int counter = 0;
  bool was_central_jet = false;
  for (const Jet* aJet : theJets) {
    counter++;
    double etjet  = aJet->et();
    double etajet = aJet->eta();
    all_jet0_et  = etjet;
    all_jet0_eta = etajet;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jet eta: " << etajet << " Eta cut: " << eta_range << endreq;
    if( std::fabs(etajet)<= eta_range ) {
      was_central_jet = true;
      passed_jet0_et  = etjet;
      passed_jet0_eta = etajet;
    }
    if( counter >= 1 ) break; // do this for the leading jet only
    
  }

  
  //Et Cut
  if( was_central_jet == true) {
    pass = true;
    m_accepted++;
    
    //    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Event accepted ! " << endreq;
  } else {
    m_rejected++;
    //    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Event rejected !" << endreq;
  }
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Jets from RoI: ";
    //  const TrigRoiDescriptor* roiDescriptor = 0;
    //  if ( getFeature(outputTE, roiDescriptor)==HLT::OK || roiDescriptor == 0 ) {      
    //    msg() << MSG::DEBUG << *roiDescriptor; 
    //  }else {
    //    msg() << MSG::DEBUG << "unknown! "; 
    //  }
    msg() << MSG::DEBUG << " with leading Jet of eta: " << (*theJets.begin())->eta() << " cut: " << eta_range << (pass ? " accepted ": " rejected ") << endreq;
    
  }
    
  // Time total TrigEFCaloHypo execution time.
  // -------------------------------------

  //    if (m_timersvc) m_timers[0]->stop();

  return HLT::OK;

}
