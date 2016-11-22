/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFJetHypo.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigEFCaloHypo
//
// AUTHOR:
//
// ********************************************************************
//
#include <algorithm>
#include <list>
#include <iterator>
#include <sstream>
#include <cmath>
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "TrigSteeringEvent/Enums.h"
#include "TrigJetHypo/TrigEFJetHypo.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "JetUtils/JetCaloQualityUtils.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigPassBits.h"

class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFJetHypo::TrigEFJetHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("Etcut",   m_EtCut = 40*CLHEP::GeV); // Default: 40 GeV
  declareProperty("etaMincut",   m_etaMinCut = 0 ); // Default: 0
  declareProperty("etaMaxcut",   m_etaMaxCut = 10 ); // Default: 10
  declareProperty("Multiplicitycut", m_multiplicitycut=1);
  declareProperty("doMonitoring", m_doMonitoring = false );
  declareProperty("AcceptAll",      m_acceptAll=false);
  //declareProperty("histoPath", m_path = "/stat/Monitoring/EventFilter" );

  declareProperty("EtThresholds",   m_EtThresholdsInput ); // Default: 40 GeV
  
  // cleaning
  declareProperty("doBasicCleaning", m_doBasicCleaning = false );
  declareProperty("triggerOnBad", m_triggerOnBad = false );
  
  declareProperty("n90CleaningThreshold", m_n90Threshold = 2 );
  declareProperty("presamplerCleaningThreshold", m_presamplerThreshold = 0.9 );
  declareProperty("negativeECleaningThreshold", m_negativeEThreshold = -60e3 );	// 60 GeV
  declareProperty("qmeanCleaningThreshold", m_qmeanThreshold = 0.8 );
  declareProperty("HECQCleaningThreshold", m_hecQThreshold = 0.5 );
  declareProperty("HECfCleaningThreshold", m_hecFThreshold = 0.5 );
  declareProperty("LArQCleaningThreshold", m_larQThreshold = 0.8 );
  declareProperty("EMfCleaningThreshold", m_emFThreshold = 0.95 );

  declareMonitoredVariable("CutCounter", m_cutCounter);

  // Monitored variables...
  declareMonitoredVariable("NJet", m_njet);
  declareMonitoredVariable("Et", m_et);
  declareMonitoredVariable("Eta", m_eta);
  declareMonitoredVariable("Phi", m_phi);
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFJetHypo::~TrigEFJetHypo()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetHypo::hltInitialize()
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
    TrigTimer* tmp = m_timersvc->addItem("TrigEFJetHypo.TrigEFJetHypoTot");
    m_timers.push_back(tmp);
  }

  if ( m_EtThresholdsInput.empty() ) { 
    for ( unsigned i=0 ; i<m_multiplicitycut ; i++ ){
      m_EtThresholds.push_back(m_EtCut);
    }      
  } else { 
    for  ( unsigned i=0 ; i<m_EtThresholdsInput.size() ; i++ ){
      m_EtThresholds.push_back(m_EtThresholdsInput[i]);
    }

    if ( m_EtThresholds.size()!=m_multiplicitycut ) {
      /// if not enough thresholds for the required number of jets... 
      /// pad with zeros to the required number of thresholds just in case... 
      for  ( unsigned i=m_EtThresholds.size() ; i<m_multiplicitycut ; i++ ){
        m_EtThresholds.push_back(0);
      }
      
      msg() << MSG::ERROR
            << name()
            << ": mismatch between number of jets required and number of thresholds: "
            << m_multiplicitycut 
            << " jets requested but only "
            << m_EtThresholdsInput.size()
            << " provided."
            << endmsg;    
  
      return HLT::ErrorCode(HLT::Action::ABORT_JOB,HLT::Reason::BAD_JOB_SETUP);
    }
    
    bool sorted = true;
    for  ( unsigned i=1 ; i<m_EtThresholds.size() ; i++ ){
      if ( m_EtThresholds[i]>m_EtThresholds[i-1] ) { 
        sorted = false;
        msg() << MSG::WARNING
              << name()
              << ": Threshold "
              << i 
              << " (" 
              << m_EtThresholds[i] 
              << " MeV) greater that threshold " 
              << i-1 
              << " (" 
              << m_EtThresholds[i-1]
              << " MeV), thresholds should be in DECREASING order "
              << endmsg;    
      }
    }

    if  ( !sorted ) { 
      msg() << MSG::WARNING 
            << name() 
            << ": sorting thresholds into reverse order" 
            << endmsg; 
      std::sort( m_EtThresholds.begin(), m_EtThresholds.end() );
      std::reverse( m_EtThresholds.begin(), m_EtThresholds.end() );
    }
  }
  
  for ( unsigned i=0 ; i<m_EtThresholds.size() ; i++ ) {
    msg() << MSG::INFO 
          << name() 
          << "\t EtThreshold[" 
          << i 
          << "]=" 
          << m_EtThresholds[i] 
          << " MeV" 
          << endmsg;
  }
  
  return HLT::OK;
  
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetHypo::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endmsg;
  return HLT::OK;

}


struct DescendingEt:std::binary_function<const xAOD::Jet*,
                                         const xAOD::Jet*,
                                         bool> {
  bool operator()(const xAOD::Jet* l, const xAOD::Jet* r)  const {
    return l->p4().Et() > r->p4().Et();
  }
};

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetHypo::hltExecute(const HLT::TriggerElement* outputTE,
                                         bool& pass) {

  ATH_MSG_DEBUG("Executing " << name() << "...");
  ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());

  m_cutCounter = -1;

  pass=false;

  m_njet = 0.0;
  m_et = -99000.;
  m_eta = -99.;
  m_phi = -99.;

  // get JetCollections from the trigger element:
  //--------------------------------------------------

  // xAOD conversion const JetCollection* outJets(0);
  const xAOD::JetContainer* outJets(0);
  HLT::ErrorCode ec = getFeature(outputTE, outJets);

  if(ec!=HLT::OK) {
    ATH_MSG_WARNING("Failed to get JetCollection");
    return ec;
  } else {
    ATH_MSG_DEBUG("Obtained JetContainer");
  }

  m_cutCounter = 0;

  if(outJets == 0){
    ATH_MSG_WARNING("Jet collection pointer is 0");
    m_errors++;
    if (m_acceptAll){m_cutCounter = 1;}
    return HLT::ERROR;
  }

  std::vector<const xAOD::Jet*> theJets(outJets->begin(), outJets->end());

  std::size_t njets = theJets.size();
  if( njets == 0 ){
    ATH_MSG_DEBUG("JetCollection is empty");
    m_errors++;
    if (m_acceptAll) {m_cutCounter = 1;}
    return HLT::OK;
  } else {
    ATH_MSG_DEBUG("JetCollection contains " << njets <<"jets");
  }

  std::sort (theJets.begin(), theJets.end(), DescendingEt());
  // debug: check the sort order is ok
  if (theJets.size() > 1){
    if (theJets[0]->p4().Et() < theJets[1]->p4().Et()){
      ATH_MSG_ERROR("Coding error: JetCollection not ordered ");
      return HLT::ERROR;
    }
  }
  ///  now add the TrigPassBits for the jets to specify whether each jet 
  ///  passes the hypo etc

  TrigPassBits* bits = HLT::makeTrigPassBits(outJets);


  unsigned int cut_multiplicity = 0;
  unsigned int multiplicity = 0;
  for (const xAOD::Jet* aJet : theJets) {
    double etjet = aJet->p4().Et();
    double etajet = aJet->eta();

    // xAOD conversion
    // double n90jet = aJet->getMoment("n90");
    // double presamplerfractionjet = aJet->getMoment("presamplerFraction");
    // double negEjet = aJet->getMoment("NegativeE");

    // ----------------------------------------------
    // begin cleaning
    bool isClean = true;
    if(m_doBasicCleaning) {
      bool ok = this -> doBasicCleaning(aJet, isClean);
      if (not ok){ 
        ATH_MSG_DEBUG("Jet cleaning failed");
        return HLT::ERROR;
      }
    }
    bool cleaningAccept = m_triggerOnBad ? !isClean : isClean;
    
    // xAOD conversion: no setting on a constant jet*
    // aJet->setMoment("triggeredCut", triggered_cut);
    
    // end cleaning
    // ----------------------------------------------
  
    if( etjet > m_EtThresholds[cut_multiplicity] &&
        std::fabs(etajet)>=m_etaMinCut &&
        std::fabs(etajet)<m_etaMaxCut &&
        cleaningAccept) {
      
      multiplicity++;
      /// flag all jets as having passed or not for single jet chains
      if ( m_multiplicitycut!=1  ) cut_multiplicity++;
      
      /// flag this jet as having passed
      
      HLT::markPassing( bits, aJet, outJets ); 
      //  careful here, the threshold vector m_EtThresholds only has
      // m_multiplicitycut  thresholds, so mustn't try to test the et for more
      // than that many jets or the code will seg fault    
      if( multiplicity >= m_multiplicitycut && m_multiplicitycut>1 ) break;
    }
  }
  
  // Et Cut
  if(multiplicity >= m_multiplicitycut || m_acceptAll) {
    pass = true;
    m_accepted++;
    m_cutCounter=1;
    
    if ( multiplicity >= m_multiplicitycut ) {
      m_njet = multiplicity;
      
      const xAOD::Jet *aJet = *(theJets.begin());    
      m_et = aJet->p4().Et();
      m_eta = aJet->eta();
      m_phi = aJet->phi();
    }
    ATH_MSG_DEBUG("Event accepted");
  } else {
    m_rejected++;
    ATH_MSG_DEBUG("Event rejected");
    
  }

  if(msgLvl() <= MSG::DEBUG) {

    ATH_MSG_DEBUG("found multiplicity: "
                  << multiplicity
                  << "\t multiplicity cut: "
                  << m_multiplicitycut
                  << (pass ? " accepted ": " rejected "));

    unsigned int i = 0;
    for (const xAOD::Jet* jet : theJets) {
      if (i >= m_EtThresholds.size()) break;
      ATH_MSG_DEBUG("    with Jet " 
                    << i
                    << " of Et: " 
                    << jet->p4().Et() 
                    << "\t Et cut: " 
                    << m_EtThresholds[i] 
                    << (pass ? " accepted ": " rejected "));
      ++i;
    }
  }
    
  // attach the trigger bits to the output trigger element
  if ( attachBits(outputTE, bits ) != HLT::OK ) {
    ATH_MSG_ERROR("Problem attaching TrigPassBits to the trigger element");
  }


  return HLT::OK;
}


bool TrigEFJetHypo::getJetAttribute(const std::string& label,
                               float& val,
                               const xAOD::Jet* j){
  /* retrieve jet attributes whuich of type float*/
    
  bool attr_ok = j->getAttribute<float>(label, val);
  if (not attr_ok) {
    ATH_MSG_DEBUG("retrieval of jet attribute " << label << " failed");
    m_errors++;
  }

  if (m_acceptAll) m_cutCounter = 1;
  return attr_ok;
}


bool TrigEFJetHypo::doBasicCleaning(const xAOD::Jet* aJet, bool& isClean){
  /* make cuts on jet attributes to select clean jets */
  
  /* triggered_Cut was set the jet 2012 code
  for now calculate but ignore */
  
  int triggered_cut(0);  
  isClean = true;
  bool ok;

  float n90jet;
  ok = this->getJetAttribute("n90", n90jet, aJet);
  if (not ok) {return false;}
  
  if(n90jet < m_n90Threshold){
    ATH_MSG_DEBUG("EF jet cleaning: n90 < " << m_n90Threshold);
    triggered_cut += 1;
    isClean = false;
  }
    	
  float presamplerfractionjet;
  ok = getJetAttribute("presamplerFraction", presamplerfractionjet, aJet);
  if (not ok) {return false;}
  
  if(presamplerfractionjet > m_presamplerThreshold) {
    ATH_MSG_DEBUG(" EF jet cleaning: presampler fraction > "
                  << m_presamplerThreshold);
    triggered_cut += 2;
    isClean = false;
  }
    	
  float negEjet;
  ok = getJetAttribute("NegativeE", negEjet, aJet);
  if (not ok) {return false;}
  
  if(negEjet < m_negativeEThreshold){
    ATH_MSG_WARNING(" EF jet cleaning: Negative energy < "
                    << m_negativeEThreshold);
    triggered_cut += 32;
    isClean = false;
  }

  return isClean;
}
