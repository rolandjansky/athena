/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigEFRazorAllTE.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR: Lawrence Lee, based on The DPhiMetJet hypo by others
//
// ********************************************************************
//
#include <list>
#include <iterator>
#include <sstream>
#include <cmath>
#include <string>
#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"

#include "TrigJetHypo/TrigEFRazorAllTE.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
#include "FourMomUtils/P4DescendingSorters.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TrigSteeringEvent/TrigPassBits.h"

#include "TrigMissingEtEvent/TrigMissingET.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include "xAODJet/Jet.h"

class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFRazorAllTE::TrigEFRazorAllTE(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator) {

  declareProperty("Razorcut",     m_RazorCut        = 300*CLHEP::GeV );   // Default: 300 GeV
  declareProperty("doMonitoring", m_doMonitoring = true  );

  // Monitored variables...
  declareMonitoredVariable("cutCounter"  , m_cutCounter  );

  declareMonitoredVariable("passedJetCuts_phi_0", m_passedJetCuts_jetphi_0  );
  declareMonitoredVariable("passedJetCuts_eta_0", m_passedJetCuts_jeteta_0  );
  declareMonitoredVariable("passedJetCuts_et_0" , m_passedJetCuts_jetet_0   );

  declareMonitoredVariable("passedJetCuts_phi_1", m_passedJetCuts_jetphi_1  );
  declareMonitoredVariable("passedJetCuts_eta_1", m_passedJetCuts_jeteta_1  );
  declareMonitoredVariable("passedJetCuts_et_1" , m_passedJetCuts_jetet_1   );

  declareMonitoredVariable("Razor"	         , m_Razor           );
  declareMonitoredVariable("Razor_passedRazor" , m_Razor_passedRazor  );
  declareMonitoredVariable("nJets"	         , m_nJets            );

  declareMonitoredVariable("gaminvRp1"   ,  m_gaminvRp1 );     
  declareMonitoredVariable("shatR"   ,    m_shatR   );  
  declareMonitoredVariable("mdeltaR"   ,  m_mdeltaR );     
  declareMonitoredVariable("cosptR"   ,   m_cosptR  );   
  declareMonitoredVariable("Minv2"   ,    m_Minv2   );  
  declareMonitoredVariable("Einv"   ,     m_Einv    ); 

}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFRazorAllTE::~TrigEFRazorAllTE()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////
//

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFRazorAllTE::hltInitialize()
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
    TrigTimer* tmp = m_timersvc->addItem("TrigEFRazorAllTE.TrigEFRazorAllTETot");
    m_timers.push_back(tmp);
  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization:" << endreq;
    msg() << MSG::DEBUG	  << "RazorCut: " << m_RazorCut << endreq;
  }  

  m_accepted=0;
  m_rejected=0;
  m_errors=0;

  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFRazorAllTE::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endreq;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endreq;
  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFRazorAllTE::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
             unsigned int output){
  // ----------------------------------------------------------------------


  // Time total TrigEFRazorAllTE execution time.
  // -------------------------------------
  if (m_timersvc) m_timers[0]->start();

  m_cutCounter = -1;

  bool pass=false;

  msg() << MSG::DEBUG << "***  Executing this TrigJetHypo : " << name() << endreq;


  // Let's get the collections from the input TEs //////////////////////////////////////

  // Check size of vector of TEs
  if (tes_in.size() < 1) {
    msg() << MSG::WARNING << "No TriggerElements provided -> do nothing" << endreq;
    return HLT::MISSING_FEATURE; // since no TE are found while there should be something (why would the chain start otherwise?)
  }
  else if (tes_in.size() < 2) {
    msg() << MSG::WARNING << "Less than 2 TriggerElements provided -> do nothing" << endreq;
    return HLT::MISSING_FEATURE; // since no TE are found while there should be something (why would the chain start otherwise?)
  }


  // Check size of individual TEs
  if(tes_in[0].size() == 0) {
    msg() << MSG::WARNING << "No MET TE found" << endreq;
    return HLT::MISSING_FEATURE; // put here not OK but something that is not aborting further execution
  }
  if(tes_in[1].size() == 0) {
    msg() << MSG::WARNING << "No jet TE found" << endreq;
    return HLT::MISSING_FEATURE; // put here not OK but something that is not aborting further execution
  }

  beforeExecMonitors().ignore();

  HLT::TEVec allTEs;

  // Retrieve MET from tes_in[0]
  // std::vector<const TrigMissingET*> vectorMissingET;
  std::vector<const xAOD::TrigMissingET*> vectorMissingET;
  HLT::ErrorCode statMET = getFeatures(tes_in[0].front(), vectorMissingET );
  if(statMET != HLT::OK) {
    msg() << MSG::WARNING << " Failed to get vectorMissingETs " << endreq;
    return HLT::OK;
  }
  if(vectorMissingET.size()==0){
    msg() << MSG::WARNING << " Failed to get vectorMissingETs " << endreq;
    return HLT::MISSING_FEATURE;
  }

  if ( (tes_in.size()>0) &&  (tes_in[0].size()>0) ) allTEs.push_back( tes_in[0][0] );
    
  // const TrigMissingET* efmet = vectorMissingET.front();
  const xAOD::TrigMissingET* efmet = vectorMissingET.front();

  // Retrieve Hemisphere Jets from tes_in[1]
  const xAOD::JetContainer* outJets(0);
  HLT::ErrorCode ec = getFeature(tes_in[1].front(), outJets, "TrigHLTJetHemisphereRec");
  if( outJets == 0 ){
    msg() << MSG::WARNING << " Got no JetCollections associated to the TE! " << endreq;
    m_errors++;
    return HLT::OK;
  }
  if ( (tes_in.size()>0) && (tes_in[1].size()>0) ) allTEs.push_back( tes_in[1][0] );

  // Got all the input collections /////////////////////////////////////////////////////


  // Creation of internal inputs and some sanity checks... ////////////////////////////

  TVector3 MET(efmet->ex()/1000., efmet->ey()/1000., 0.0);

  // JetCollection -> jetcollection_t
  std::vector<const xAOD::Jet*> theJets(outJets->begin(), outJets->end());

  //check size of JetCollection not empty
  if( theJets.size() == 0){
    msg()<< MSG::WARNING << " Size of JetCollection is 0! " << endreq;
    m_errors++;
    if (m_timersvc) m_timers[0]->stop();
    return HLT::OK;
  }

  if( theJets.size() != 2){
    msg()<< MSG::WARNING << " Size of JetCollection is 2! - Razor variables require exactly two megajets" << endreq;
    m_errors++;
    if (m_timersvc) m_timers[0]->stop();
    return HLT::OK;
  }

  if( theJets.at(0)->jetP4().E()==0. || theJets.at(1)->jetP4().E()==0.  ){
    msg()<< MSG::WARNING << " Energy-less hemisphere. Something's wrong" << endreq;
    m_errors++;
    if (m_timersvc) m_timers[0]->stop();
    return HLT::OK;
  }

  m_cutCounter = 1;

  m_nJets            = 0   ;

  m_Razor            = -999.;
  m_Razor_passedRazor   = -999.;

  m_passedJetCuts_jetphi_0 = -999.;
  m_passedJetCuts_jeteta_0 = -999.;
  m_passedJetCuts_jetet_0  = -999.;

  m_passedJetCuts_jetphi_1 = -999.;
  m_passedJetCuts_jeteta_1 = -999.;
  m_passedJetCuts_jetet_1  = -999.;

  m_gaminvRp1 = -999.;
  m_shatR = -999.;
  m_mdeltaR = -999.;
  m_cosptR = -999.;

  m_Minv2 = -999.;
  m_Einv = -999.;

  ///// Calculate (super) Razor Observables ////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  // With update to include massive jets


  m_nJets = theJets.size();
  
  TLorentzVector J1,J2;
  J1.SetPtEtaPhiM(  theJets.at(0)->jetP4().Pt()/1000.,
                    theJets.at(0)->jetP4().Eta(),
                    theJets.at(0)->jetP4().Phi(),
                    theJets.at(0)->jetP4().M()/1000.
                 );
  J2.SetPtEtaPhiM(  theJets.at(1)->jetP4().Pt()/1000.,
                    theJets.at(1)->jetP4().Eta(),
                    theJets.at(1)->jetP4().Phi(),
                    theJets.at(1)->jetP4().M()/1000.
                 );


  m_passedJetCuts_jetphi_0 = J1.Phi();
  m_passedJetCuts_jeteta_0 = J1.Eta();
  m_passedJetCuts_jetet_0  = J1.Et();

  m_passedJetCuts_jetphi_1 = J2.Phi();
  m_passedJetCuts_jeteta_1 = J2.Eta();
  m_passedJetCuts_jetet_1  = J2.Et();

  J1.SetVectM(J1.Vect(),0.0);
  J2.SetVectM(J2.Vect(),0.0);

  TVector3 vBETA_z = (1./(J1.E()+J2.E()))*(J1+J2).Vect();
  vBETA_z.SetX(0.0);
  vBETA_z.SetY(0.0);

  //transformation from lab frame to approximate rest frame along beam-axis
  J1.Boost(-vBETA_z);
  J2.Boost(-vBETA_z);

  TVector3 pT_CM = (J1+J2).Vect() + MET;
  pT_CM.SetZ(0.0); //should be redundant...

  m_Minv2 = (J1+J2).M2() - 4.*J1.M()*J2.M();
  m_Einv = sqrt(MET.Mag2()+m_Minv2);


  //////////////////////
  // definition of m_shatR
  //////////////////////
  m_shatR = sqrt( ((J1+J2).E()+m_Einv)*((J1+J2).E()+m_Einv) - pT_CM.Mag2() );

  TVector3 vBETA_R = (1./sqrt(pT_CM.Mag2() + m_shatR*m_shatR))*pT_CM;

  //transformation from lab frame to R frame
  J1.Boost(-vBETA_R);
  J2.Boost(-vBETA_R);

  /////////////
  //
  // R-frame
  //
  /////////////

  TVector3 vBETA_Rp1 = (1./(J1.E()+J2.E()))*(J1.Vect() - J2.Vect());

  ////////////////////////
  // definition of m_gaminvR
  ////////////////////////

  m_gaminvRp1 = 1./sqrt(1.-vBETA_Rp1.Mag2());


  // Define "razor" cut as m_gaminvRp1 * m_shatR
  // This is "Pi" according to the talks. 
  // Most versions involve some offsets on the two factors. 
  // Will need to be put in if those are nonzero.

  m_Razor = m_gaminvRp1 * m_shatR;

  // Trigger cuts on this "razor" cut value

  if (m_Razor > m_RazorCut) {
    m_Razor_passedRazor = m_Razor;

    pass = true;
    m_accepted++;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << ">>>>> TRIGGER PASSED <<<<<       Razor: " << m_Razor << " PASSED Razor cut of " << m_RazorCut << endreq;
    m_cutCounter = 2;
  }
  else {
    pass = false;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << ">>>>> TRIGGER FAILED <<<<<       Razor: " << m_Razor << " FAILED Razor cut of " << m_RazorCut << endreq;
  }


  HLT::TriggerElement* out_te = config()->getNavigation()->addNode(allTEs, output);    
  out_te->setActiveState(false);  
  
  if (pass==true){
    out_te->setActiveState(true);
  }

  

  // Time total TrigEFRazorAllTE execution time.
  // -------------------------------------

  if (m_timersvc) m_timers[0]->stop();

  afterExecMonitors().ignore();


  return HLT::OK;

}
