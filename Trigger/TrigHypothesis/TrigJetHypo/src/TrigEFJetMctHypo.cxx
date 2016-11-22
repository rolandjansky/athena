/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFJetMctHypo.cxx

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

#include "TrigJetHypo/TrigEFJetMctHypo.h"

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
TrigEFJetMctHypo::TrigEFJetMctHypo(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator) {

  declareProperty("MctCut",   m_MctCut = 150*Units::GeV); // Default: 150 GeV
  declareProperty("Etcut1",   m_EtCut1 = 30*Units::GeV); // Default: NO CUT
  declareProperty("Etcut2",   m_EtCut2 = 30*Units::GeV); // Default: NO CUT
  declareProperty("etaMincut",   m_etaMinCut = 0 ); // Default: 0
  declareProperty("etaMaxcut",   m_etaMaxCut = 3.2 ); // Default: 3.2 - central jets
  declareProperty("Multiplicitycut", m_multiplicitycut=1);//Default is leadingmct
  declareProperty("doMonitoring", m_doMonitoring = false );
  declareProperty("AcceptAll",      m_acceptAll=false);
  //declareProperty("histoPath", m_path = "/stat/Monitoring/EventFilter" );



  // Monitored variables...
  declareMonitoredVariable("cutCounter"  , m_cutCounter  );


 
  declareMonitoredStdContainer("et1"	 , m_et1         );
  declareMonitoredStdContainer("et2"	 , m_et2         );
  declareMonitoredStdContainer("eta1"	 , m_eta1        );
  declareMonitoredStdContainer("eta2"	 , m_eta2        );
  declareMonitoredStdContainer("phi1"	 , m_phi1        );
  declareMonitoredStdContainer("phi2"	 , m_phi2        );

  declareMonitoredVariable(    "passedJetCuts_maxmct"	 , m_passedJetCuts_maxmct      );
  declareMonitoredStdContainer("passedJetCuts_mct"	 , m_passedJetCuts_mct         );
  declareMonitoredStdContainer("passedJetCuts_dphi"	 , m_passedJetCuts_dphi        );
  declareMonitoredStdContainer("passedJetCuts_et1"	 , m_passedJetCuts_et1         );
  declareMonitoredStdContainer("passedJetCuts_et2"	 , m_passedJetCuts_et2         );
  declareMonitoredStdContainer("passedJetCuts_eta1"	 , m_passedJetCuts_eta1        );
  declareMonitoredStdContainer("passedJetCuts_eta2"	 , m_passedJetCuts_eta2        );
  declareMonitoredStdContainer("passedJetCuts_phi1"	 , m_passedJetCuts_phi1        );
  declareMonitoredStdContainer("passedJetCuts_phi2"	 , m_passedJetCuts_phi2        );

  declareMonitoredVariable(    "passedMctCut_maxmct", m_passedMctCut_maxmct);
  declareMonitoredStdContainer("passedMctCut_mct"   , m_passedMctCut_mct   );
  declareMonitoredStdContainer("passedMctCut_dphi"  , m_passedMctCut_dphi  );
  declareMonitoredStdContainer("passedMctCut_et1"   , m_passedMctCut_et1   );
  declareMonitoredStdContainer("passedMctCut_et2"   , m_passedMctCut_et2   );
  declareMonitoredStdContainer("passedMctCut_eta1"  , m_passedMctCut_eta1  );
  declareMonitoredStdContainer("passedMctCut_eta2"  , m_passedMctCut_eta2  );
  declareMonitoredStdContainer("passedMctCut_phi1"  , m_passedMctCut_phi1  );
  declareMonitoredStdContainer("passedMctCut_phi2"  , m_passedMctCut_phi2  );


  declareMonitoredVariable("nComb"       , m_nComb       );
  declareMonitoredVariable("nJet"        , m_nJet        );



  declareMonitoredVariable("passednComb"     , m_passednComb     );

}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFJetMctHypo::~TrigEFJetMctHypo()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetMctHypo::hltInitialize()
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

    TrigTimer* tmp = m_timersvc->addItem("TrigEFJetMctHypo.TrigEFJetMctHypoTot");
    m_timers.push_back(tmp);
  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization:" << endmsg;
    if(m_multiplicitycut<0){
      msg() << MSG::DEBUG << "REGTEST ANYMCT Cut values: JetMctCut: " << m_MctCut; 
    }else if(m_multiplicitycut==1){
      msg() << MSG::DEBUG << "REGTEST LEADINGMCT Cut values: JetMctCut: " << m_MctCut; 
    }else{
      msg() << MSG::DEBUG << "REGTEST MULTIPLICITY==" << m_multiplicitycut << "!!! Cut values: JetMctCut: " << m_MctCut; 
    }
    msg() << MSG::DEBUG	  << ", ptHardJetCut: " << m_EtCut1 << ", ptSoftJetCut: " << m_EtCut2 << endmsg;
  }
  
  if(m_multiplicitycut>1){
    if (msgLvl()<=MSG::WARNING) msg() << MSG::WARNING << "UNKNOWN CONFIGURATION: Mulitplicity cut set to " <<  m_multiplicitycut << endmsg; 
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);    
  }
  

  m_accepted=0;
  m_rejected=0;
  m_errors=0;

  return HLT::OK;

}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetMctHypo::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endmsg;
  return HLT::OK;

}


// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFJetMctHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass) {
  // ----------------------------------------------------------------------


  // Time total TrigEFJetMctHypo execution time.
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
    msg()<< MSG::WARNING << " Size of JetCollection is not empty but nJet < 2 - Cannot compute MCT" << endmsg;
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

  m_passedJetCuts_mct.clear();
  m_passedJetCuts_dphi.clear();
  m_passedJetCuts_et1.clear();
  m_passedJetCuts_et2.clear();
  m_passedJetCuts_eta1.clear();
  m_passedJetCuts_eta2.clear();
  m_passedJetCuts_phi1.clear();
  m_passedJetCuts_phi2.clear();

  m_passedMctCut_mct.clear();
  m_passedMctCut_dphi.clear();
  m_passedMctCut_et1.clear();
  m_passedMctCut_et2.clear();
  m_passedMctCut_eta1.clear();
  m_passedMctCut_eta2.clear();
  m_passedMctCut_phi1.clear();
  m_passedMctCut_phi2.clear();


  //Variable finally triggered on
  double maxmct = -999.;


  double passedMctCut_maxmct = -999.;  
 
  //   double passedMctCut_mct = -999.;  
  //   double passedMctCut_dphi = -999.;  
  //   double passedMctCut_jet1Et = -999.;  
  //   double passedMctCut_jet2Et = -999.;   
  //   double passedMctCut_jet1Eta = -999.;  
  //   double passedMctCut_jet2Eta = -999.;  
  //   double passedMctCut_jet1Phi = -999.;  
  //   double passedMctCut_jet2Phi = -999.;  


  int multiplicity = 0;
  int passedmultiplicity = 0;


  for(auto firstJet = theJets.begin(); firstJet!=theJets.end(); firstJet++) {

    bool innerLoopBreak=false;

    const Jet *aJet = *firstJet;
    
    firstJet++;
    auto secondJetBegin = firstJet;
    firstJet--;
    
    for(auto secondJet = secondJetBegin ; secondJet!=theJets.end(); secondJet++) {
      const Jet *bJet = *secondJet;
      


      double jet1Et = -999.;  
      double jet1Eta = -999.;  
      double jet1Phi = -999.;  
    
      jet1Et = aJet->et(); 
      jet1Eta= aJet->eta();
      jet1Phi= aJet->phi();



      double jet2Et = -999.;  
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
      if(jet1Et > m_EtCut1 && std::fabs(jet1Eta)>=m_etaMinCut && std::fabs(jet1Eta)<m_etaMaxCut){
	
	if(msgLvl() <= MSG::DEBUG)msg() << MSG::DEBUG << "Jet1 - Et: " << jet1Et << " Eta: " << jet1Eta << " PASSED Et cut: " << m_EtCut1 << " |Eta| cut: " << m_etaMaxCut << endmsg;
	

	//Jet2 Cuts
	if(jet2Et > m_EtCut2 && std::fabs(jet2Eta)>=m_etaMinCut && std::fabs(jet2Eta)<m_etaMaxCut){
	  
	  if(msgLvl() <= MSG::DEBUG)msg() << MSG::DEBUG << "Jet2 - Et: " << jet2Et << " Eta: " << jet2Eta << " PASSED Et cut: " << m_EtCut2 << " |Eta| cut: " << m_etaMaxCut << endmsg;
	     
	  double mct = -999.;
	  double dphi = -999.;

	  //Monitoring	     
	  m_passedJetCuts_et1.push_back(jet1Et/Units::GeV);
	  m_passedJetCuts_eta1.push_back(jet1Eta);
	  m_passedJetCuts_phi1.push_back(jet1Phi);

	  m_passedJetCuts_et2.push_back(jet2Et/Units::GeV);
	  m_passedJetCuts_eta2.push_back(jet2Eta);
	  m_passedJetCuts_phi2.push_back(jet2Phi);


	  
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


	  //MCT calculation    
	  mct = sqrt(2.*jet1Et*jet2Et*(1+cos(dphi)));
	  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Value of Mct: " << mct << endmsg;
	  
	  //Monitoring
	  m_passedJetCuts_dphi.push_back(dphi);
	  m_passedJetCuts_mct.push_back(mct/Units::GeV);
    

	  if(mct > maxmct)maxmct=mct;    
    
	  //Mct Cut
	  if( mct > m_MctCut){	  

	    /// flag these jets as having passed
	    HLT::markPassing( bits, aJet, outJets ); 
	    HLT::markPassing( bits, bJet, outJets ); 
	    ///  careful here, the threshold vector m_EtThresholds only has m_multiplicitycut 
	    ///  thresholds, so mustn't try to test the et for more than that many jets or 
	    ///  the code will seg fault    


	    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Mct: " << mct << " PASSED Mct cut of " << m_MctCut << endmsg;

	    m_cutCounter=3;

	    passedMctCut_maxmct = maxmct;

	    m_passedMctCut_mct.push_back(mct/Units::GeV);
	    m_passedMctCut_dphi.push_back(dphi);
	    
	    m_passedMctCut_et1.push_back(jet1Et/Units::GeV);
	    m_passedMctCut_eta1.push_back(jet1Eta);
	    m_passedMctCut_phi1.push_back(jet1Phi);
	    
	    m_passedMctCut_et2.push_back(jet2Et/Units::GeV);
	    m_passedMctCut_eta2.push_back(jet2Eta);
	    m_passedMctCut_phi2.push_back(jet2Phi);



	    passedmultiplicity++;

	  }else{
	    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Mct: " << mct << " FAILED Mct cut of " << m_MctCut << endmsg;
	  }


	  //if LeadingMCT break - should only ever consider 1 pair of jets that pass ET and eta requirements
	  if(m_multiplicitycut==1){
	    innerLoopBreak=true;
	    break;
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



  //Multiplicity check
  if( m_multiplicitycut==1 && multiplicity > m_multiplicitycut) {
    if (msgLvl()<=MSG::WARNING) msg() << MSG::WARNING << "There are " <<  multiplicity << " combinations of jets being considered, LEADINGMCT should only consider "<< m_multiplicitycut << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
    //    afterExecMonitors().ignore();
  }

  //Monitoring


  m_passedJetCuts_maxmct = maxmct/Units::GeV;  
//   m_mct   = mct; 
//   m_dphi  = dphi;  
//   m_et1  = jet1Et;  
//   m_et2  = jet2Et;  
//   m_eta1 = jet1Eta;  
//   m_eta2 = jet2Eta;  
//   m_phi1 = jet1Phi;  
//   m_phi2 = jet2Phi;  


  m_passedMctCut_maxmct = passedMctCut_maxmct/Units::GeV;

//   m_passedMctCut_mct    = passedMctCut_mct;  
//   m_passedMctCut_dphi = passedMctCut_dphi;  
//   m_passedMctCut_et1  = passedMctCut_jet1Et;  
//   m_passedMctCut_et2  = passedMctCut_jet2Et;   
//   m_passedMctCut_eta1 = passedMctCut_jet1Eta;  
//   m_passedMctCut_eta2 = passedMctCut_jet2Eta;  
//   m_passedMctCut_phi1 = passedMctCut_jet1Phi;  
//   m_passedMctCut_phi2 = passedMctCut_jet2Phi;  

  m_nJet = nJet;  
  m_nComb = multiplicity;
  m_passednComb = passedmultiplicity;





  //TRIGGER DECISION
  if( maxmct > m_MctCut || m_acceptAll) {

    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << ">>>>> TRIGGER PASSED <<<<<       Mct: " << maxmct << " PASSED Mct cut of " << m_MctCut << endmsg;

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
    
    //msg() << MSG::DEBUG << "TrigPassBits: "<<bits.size()<<"jets: "<<theJets.size()<<endmsg;

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
