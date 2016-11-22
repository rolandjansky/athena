/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFJetDphiHypo.cxx

// PACKAGE:  Trigger/TrigHypothesis/TrigEFCaloHypo
//
// AUTHOR: Anna Sfyrla, Josh McFayden
//
// ********************************************************************
//
#include <list>
#include <iterator>
#include <sstream>
#include <cmath>
#include <string>
#include <vector>

//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

//#include "TrigConfHLTData/HLTTriggerElement.h"

//#include "TrigSteeringEvent/TriggerElement.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigJetHypo/TrigEFJetDphiHypo.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
#include "FourMomUtils/P4DescendingSorters.h"
#include "AthenaKernel/Units.h"

#include "TrigSteeringEvent/TrigPassBits.h"

class ISvcLocator;
namespace Units = Athena::Units;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFJetDphiHypo::TrigEFJetDphiHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("DphiCut",      m_DphiCut      = 0.4   ); // Default: 0.4
  declareProperty("minDphiCut",   m_minDphiCut   = 0.3   ); // Default: 0.3
  declareProperty("Etcut1",       m_EtCut1       = 30*Units::GeV); // Default: 30 GeV
  declareProperty("Etcut2",       m_EtCut2       = 30*Units::GeV); // Default: 30 GeV
  declareProperty("etaMincut",    m_etaMinCut    = 0     ); // Default: 0
  declareProperty("etaMaxcut",    m_etaMaxCut    = 3.2   ); // Default: 3.2 - central jets only
  declareProperty("doMonitoring", m_doMonitoring = false );
  declareProperty("AcceptAll",    m_acceptAll    = false );
  //declareProperty("histoPath", m_path = "/stat/Monitoring/EventFilter" );



  // Monitored variables...
  declareMonitoredVariable("cutCounter"  , m_cutCounter  );


  declareMonitoredVariable("nComb"       , m_nComb       );
  declareMonitoredVariable("nJet"        , m_nJet        );

  declareMonitoredStdContainer("et1"	 , m_et1         );
  declareMonitoredStdContainer("et2"	 , m_et2         );
  declareMonitoredStdContainer("eta1"	 , m_eta1        );
  declareMonitoredStdContainer("eta2"	 , m_eta2        );
  declareMonitoredStdContainer("phi1"	 , m_phi1        );
  declareMonitoredStdContainer("phi2"	 , m_phi2        );

  declareMonitoredVariable("passedJetCuts_minDphi"	 , m_passedJetCuts_minDphi     );
  declareMonitoredStdContainer("passedJetCuts_dphi"	  , m_passedJetCuts_dphi   );

  declareMonitoredStdContainer("passedJetCuts_et1"    , m_passedJetCuts_et1    );
  declareMonitoredStdContainer("passedJetCuts_et2"    , m_passedJetCuts_et2    );
  declareMonitoredStdContainer("passedJetCuts_eta1"   , m_passedJetCuts_eta1   );
  declareMonitoredStdContainer("passedJetCuts_eta2"   , m_passedJetCuts_eta2   );
  declareMonitoredStdContainer("passedJetCuts_phi1"   , m_passedJetCuts_phi1   );
  declareMonitoredStdContainer("passedJetCuts_phi2"   , m_passedJetCuts_phi2   );

  declareMonitoredStdContainer("passedDphiCut_dphi"   , m_passedDphiCut_dphi   );

  declareMonitoredStdContainer("passedDphiCut_et1"    , m_passedDphiCut_et1    );
  declareMonitoredStdContainer("passedDphiCut_et2"    , m_passedDphiCut_et2    );
  declareMonitoredStdContainer("passedDphiCut_eta1"   , m_passedDphiCut_eta1   );
  declareMonitoredStdContainer("passedDphiCut_eta2"   , m_passedDphiCut_eta2   );
  declareMonitoredStdContainer("passedDphiCut_phi1"   , m_passedDphiCut_phi1   );
  declareMonitoredStdContainer("passedDphiCut_phi2"   , m_passedDphiCut_phi2   );

  declareMonitoredVariable("passedDphiCut_minDphi", m_passedDphiCut_minDphi);

  declareMonitoredVariable("passednComb"       , m_passednComb       );

}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFJetDphiHypo::~TrigEFJetDphiHypo()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetDphiHypo::hltInitialize()
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

    TrigTimer* tmp = m_timersvc->addItem("TrigEFJetDphiHypo.TrigEFJetDphiHypoTot");
    m_timers.push_back(tmp);
  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization:" << endmsg;
    msg() << MSG::DEBUG << "REGTEST Dphi Cut values: JetDphiCut: " << m_minDphiCut << " < dPhi < Pi - " << m_DphiCut; 
    msg() << MSG::DEBUG	  << ", ptHardJetCut: " << m_EtCut1 << ", ptSoftJetCut: " << m_EtCut2 << endmsg;
  }
  


  m_accepted=0;
  m_rejected=0;
  m_errors=0;

  return HLT::OK;

}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetDphiHypo::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endmsg;
  return HLT::OK;

}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetDphiHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  // ----------------------------------------------------------------------


  // Time total TrigEFJetDphiHypo execution time.
  // -------------------------------------
  //  if (m_timersvc) m_timers[0]->start();

  if(msgLvl() <= MSG::DEBUG) {
    //    std::string label;
    //    TrigConf::HLTTriggerElement::getLabel (TEout->getId(), label );
    //    msg() << MSG::DEBUG <<"REGTEST:"<< name() << ": in hltExecute() on: " << label << endmsg;
  }
  m_cutCounter = -1;

  pass=false;

  
  const JetCollection* outJets(0);
  HLT::ErrorCode ec = getFeature(outputTE, outJets);

  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get JetCollections " << endmsg;
    return ec;
  }

  m_cutCounter = 0;

  if( outJets == 0 ){
    msg() << MSG::WARNING << " Got no JetCollections associated to the TE! " << endmsg;
    m_errors++;
    if (m_acceptAll) m_cutCounter = 1;
    return HLT::OK;
  }



  ///  now add the TrigPassBits for the jets to specify whether each jet 
  ///  passes the hypo etc
  TrigPassBits* bits = HLT::makeTrigPassBits( outJets );



  // JetCollection -> jetcolletcion_t
  std::vector<const Jet*> theJets(outJets->begin(), outJets->end());

  //check size of JetCollection not empty
  if( theJets.size() == 0){
    msg()<< MSG::WARNING << " Size of JetCollection is 0! " << endmsg;
    m_errors++;
    if (m_acceptAll) m_cutCounter = 1;
    //    if (m_timersvc) m_timers[0]->stop();
    return HLT::OK;
  }else  if( theJets.size() < 2 ){//check for at least 2 jets in JetCollection
    msg()<< MSG::WARNING << " Size of JetCollection is not empty but nJet < 2 - Cannot compute dPhi" << endmsg;
    m_errors++;
    if (m_acceptAll) m_cutCounter = 1;
    //    if (m_timersvc) m_timers[0]->stop();
    return HLT::OK;
  }

  //Ordering Jets
  std::sort (theJets.begin(), theJets.end(), P4Sorters::Descending::Et());

  int nJet=0;
  for (const Jet* testJet : theJets) {
    if(msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << " Jet "<< (nJet+1) << " Et: "<< testJet->et() << " Eta: "<< testJet->eta() << endmsg;
    }
    nJet++;
  }


  //clear vectors
  m_et1.clear();
  m_et2.clear();
  m_eta1.clear();
  m_eta2.clear();
  m_phi1.clear();
  m_phi2.clear();

  m_passedJetCuts_dphi.clear();
  m_passedJetCuts_et1.clear();
  m_passedJetCuts_et2.clear();
  m_passedJetCuts_eta1.clear();
  m_passedJetCuts_eta2.clear();
  m_passedJetCuts_phi1.clear();
  m_passedJetCuts_phi2.clear();

  m_passedDphiCut_dphi.clear();
  m_passedDphiCut_et1.clear();
  m_passedDphiCut_et2.clear();
  m_passedDphiCut_eta1.clear();
  m_passedDphiCut_eta2.clear();
  m_passedDphiCut_phi1.clear();
  m_passedDphiCut_phi2.clear();



  //minimum dphi from all jet pairs that is above minDphi cut
  double passedJetCuts_minDphi = 999.;


//   double passedDphiCut_dphi = -999.;  
  double passedDphiCut_minDphi = -999.;  
//   double passedDphiCut_jet1Et = -999.;  
//   double passedDphiCut_jet2Et = -999.;   
//   double passedDphiCut_jet1Eta = -999.;  
//   double passedDphiCut_jet2Eta = -999.;  
//   double passedDphiCut_jet1Phi = -999.;  
//   double passedDphiCut_jet2Phi = -999.;  


  int multiplicity = 0;
  int passedmultiplicity = 0;


  for(auto firstJet = theJets.begin(); firstJet!=theJets.end(); firstJet++) {
    const Jet *aJet = *firstJet;
    
    double jet1Et  = -999.;  
    double jet1Eta = -999.;  
    double jet1Phi = -999.;  

    jet1Et = aJet->et(); 
    jet1Eta= aJet->eta();
    jet1Phi= aJet->phi();
    


    bool innerLoopBreak=false;
    //define start of secondJet loop to be "firstJet + 1" 
    firstJet++;
    auto secondJetBegin = firstJet;
    firstJet--;
    
    for(auto secondJet = secondJetBegin ; secondJet!=theJets.end(); secondJet++) {
      const Jet *bJet = *secondJet;
      	     
      double jet2Et  = -999.;  
      double jet2Eta = -999.;  
      double jet2Phi = -999.;  
      
      jet2Et = bJet->et(); 
      jet2Eta= bJet->eta();
      jet2Phi= bJet->phi();
      

      //Monitoring
      m_et1.push_back(jet1Et/Units::GeV);
      m_eta1.push_back(jet1Eta);
      m_phi1.push_back(jet1Phi);
      
      m_et2.push_back(jet2Et/Units::GeV);
      m_eta2.push_back(jet2Eta);
      m_phi2.push_back(jet2Phi);
      

      //Et and Eta requirements
      //Jet1 Cuts
      if(aJet->et() > m_EtCut1 && std::fabs(aJet->eta())>=m_etaMinCut && std::fabs(aJet->eta())<m_etaMaxCut){
	
	if(msgLvl() <= MSG::DEBUG)msg() << MSG::DEBUG << "Jet1 - Et: " << aJet->et() << " Eta: " << aJet->eta() << " PASSED Et cut: " << m_EtCut1 << " |Eta| cut: " << m_etaMaxCut << endmsg;

	
	
	//Jet2 Cuts
	if(bJet->et() > m_EtCut2 && std::fabs(bJet->eta())>=m_etaMinCut && std::fabs(bJet->eta())<m_etaMaxCut) {
	     
	  if(msgLvl() <= MSG::DEBUG)msg() << MSG::DEBUG << "Jet2 - Et: " << bJet->et() << " Eta: " << bJet->eta() << " PASSED Et cut: " << m_EtCut2 << " |Eta| cut: " << m_etaMaxCut << endmsg;


	  //Monitoring	     
	  m_passedJetCuts_et1.push_back(jet1Et/Units::GeV);
	  m_passedJetCuts_eta1.push_back(jet1Eta);
	  m_passedJetCuts_phi1.push_back(jet1Phi);
	  
	  m_passedJetCuts_et2.push_back(jet2Et/Units::GeV);
	  m_passedJetCuts_eta2.push_back(jet2Eta);
	  m_passedJetCuts_phi2.push_back(jet2Phi);


	  double dphi = -999.;

	  //Increment no. of jet pairs considered 
	  multiplicity++;
	     
	  m_cutCounter=2;
	     
	  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << 
	    "After Jet Cuts:  Jet1 Et: " << aJet->et() << " Eta: " << aJet->eta() << 
	    " Jet2 Et: " << bJet->et() << " Eta: " << bJet->eta() << endmsg;



	  //dPhi Calculation - Compute dphi, get its absolute value, make sure it is in correct quadrant
	  dphi = jet1Phi - jet2Phi;
	  if(msgLvl() <= MSG::DEBUG) {
	    msg() << MSG::DEBUG << "Phi1: " << jet1Phi 
		  << " Phi2: " << jet2Phi << " Dphi: " << dphi << endmsg;
	  }
	  if (fabs(dphi) > M_PI ) dphi = 2*M_PI - fabs(dphi);
	  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "After correction (to get equivalent angle below PI) Dphi: " << dphi << endmsg;
	  dphi = fabs(dphi);
	  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Absolute value of Dphi: " << dphi << endmsg;
	  //Monitoring
	  m_passedJetCuts_dphi.push_back(dphi);

    
	  if(dphi < passedJetCuts_minDphi && dphi > m_minDphiCut)passedJetCuts_minDphi=dphi;    
	     
	  //Dphi Cut
	  if( dphi > m_minDphiCut && dphi < (M_PI - m_DphiCut) ){	  


	    /// flag these jets as having passed
	    HLT::markPassing( bits, aJet, outJets ); 
	    HLT::markPassing( bits, bJet, outJets ); 
	    ///  careful here, the threshold vector m_EtThresholds only has m_multiplicitycut 
	    ///  thresholds, so mustn't try to test the et for more than that many jets or 
	    ///  the code will seg fault    
	       

	    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Dphi: " << dphi << " PASSED Dphi cut of " << m_minDphiCut << " < dphi < Pi - " << m_DphiCut << " = " << (M_PI - m_DphiCut) << endmsg;
		
	    m_cutCounter=3;
		
	    //Monitoring
	    passedDphiCut_minDphi = passedJetCuts_minDphi;

	    m_passedDphiCut_dphi.push_back(dphi);
	    
	    m_passedDphiCut_et1.push_back(jet1Et/Units::GeV);
	    m_passedDphiCut_eta1.push_back(jet1Eta);
	    m_passedDphiCut_phi1.push_back(jet1Phi);
	    
	    m_passedDphiCut_et2.push_back(jet2Et/Units::GeV);
	    m_passedDphiCut_eta2.push_back(jet2Eta);
	    m_passedDphiCut_phi2.push_back(jet2Phi);


	    passedmultiplicity++;

	  }else{
	    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Dphi: " << dphi << " FAILED Dphi cut of Pi - " << m_DphiCut << " = " << (M_PI - m_DphiCut) << endmsg;
	  }


	
	  //Fail Jet2 Et Cut -> exit loops
	}else{
	  if(msgLvl() <= MSG::DEBUG){
	    msg() << MSG::DEBUG << "Jet2 - Et: " << bJet->et() << " Eta: " << bJet->eta() << " FAILED Et cut: " << m_EtCut2 << " |Eta| cut: " << m_etaMaxCut << endmsg;
	    //innerLoopBreak=true;
	    break;
	  }
	}
      
	//Fail Jet1 Et Cut -> exit loops
      }else{
	if(msgLvl() <= MSG::DEBUG){
	  msg() << MSG::DEBUG << "Jet1 - Et: " << aJet->et() << " Eta: " << aJet->eta() << " FAILED Et cut: " << m_EtCut1 << " |Eta| cut: " << m_etaMaxCut << endmsg;
	}
	//break both loops
	innerLoopBreak=true;
	break;
      }
    }
    if(innerLoopBreak)break;
  }

  //Monitoring

  //  m_dphi  = dphi;  
  m_passedJetCuts_minDphi  = passedJetCuts_minDphi;  
  
//   m_et1  = jet1Et;  
//   m_et2  = jet2Et;  
//   m_eta1 = jet1Eta;  
//   m_eta2 = jet2Eta;  
//   m_phi1 = jet1Phi;  
//   m_phi2 = jet2Phi;  


//   m_passedDphiCut_dphi = passedDphiCut_dphi;  
  m_passedDphiCut_minDphi = passedDphiCut_minDphi;  
  //   m_passedDphiCut_et1  = passedDphiCut_jet1Et;  
//   m_passedDphiCut_et2  = passedDphiCut_jet2Et;   
//   m_passedDphiCut_eta1 = passedDphiCut_jet1Eta;  
//   m_passedDphiCut_eta2 = passedDphiCut_jet2Eta;  
//   m_passedDphiCut_phi1 = passedDphiCut_jet1Phi;  
//   m_passedDphiCut_phi2 = passedDphiCut_jet2Phi;  

  m_nJet = nJet;  
  m_nComb = multiplicity;
  m_passednComb = passedmultiplicity;





  //TRIGGER DECISION
  if( (passedJetCuts_minDphi >= m_minDphiCut && passedJetCuts_minDphi < (M_PI - m_DphiCut) ) || m_acceptAll) {

    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << ">>>>> TRIGGER PASSED <<<<<       Dphi: " << passedJetCuts_minDphi << " PASSED Dphi cut of " << m_minDphiCut << " < Dphi < Pi - " << m_DphiCut << " = " << (M_PI - m_DphiCut) << endmsg;

    pass = true;
    m_accepted++;
    m_cutCounter=4;    
 

    //    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Event accepted ! " << endmsg;
  } else {
    m_rejected++;
    //    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " Event rejected !" << endmsg;
  }
  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Jets from RoI: ";
    const TrigRoiDescriptor* roiDescriptor = 0;
    if ( getFeature(outputTE, roiDescriptor)==HLT::OK || roiDescriptor == 0 ) {      
      msg() << MSG::DEBUG << *roiDescriptor; 
    }else {
      msg() << MSG::DEBUG << "unknown! "; 
    }
    msg() << MSG::DEBUG << " with leading Jet of et: " << (*theJets.begin())->et() << " cut: " << m_EtCut1 << (pass ? " accepted ": " rejected ") << endmsg;
    
    nJet=0;
    for (const Jet* testJet : theJets) {
      msg() << MSG::DEBUG << " Jet "<< (nJet+1) << " | TrigPassBit: " << HLT::isPassing( bits, testJet, outJets ) << " Et: "<< testJet->et() << " Eta: "<< testJet->eta() << endmsg;
      nJet++;
    }


  }
    



  // attach the bits
  if ( attachBits(outputTE, bits ) != HLT::OK ) {
    msg() << MSG::ERROR << "Problem attaching TrigPassBits for the Jets " << endmsg;
  }



    // Time total TrigEFCaloHypo execution time.
  // -------------------------------------

  if (m_timersvc) m_timers[0]->stop();

  return HLT::OK;

}
