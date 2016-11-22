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

#include "TrigSteeringEvent/TrigPassBits.h"

#include "TrigMissingEtEvent/TrigMissingET.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetTrigAuxContainer.h"
#include "xAODJet/Jet.h"
#include "AthenaKernel/Units.h"

namespace Units = Athena::Units;

class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigEFRazorAllTE::TrigEFRazorAllTE(const std::string& name, ISvcLocator* pSvcLocator):
  HLT::AllTEAlgo(name, pSvcLocator) {

  declareProperty("Razorcut",     m_RazorCut        = 300 );   // Default: 300 GeV
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
  msg() << MSG::INFO << "in initialize()" << endmsg;

  // Initialize timing service
  //------------------------------
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    msg() << MSG::WARNING << name()
	  << ": Unable to locate TrigTimer Service" << endmsg;
  }
  if (m_timersvc) {
    TrigTimer* tmp = m_timersvc->addItem("TrigEFRazorAllTE.TrigEFRazorAllTETot");
    m_timers.push_back(tmp);
  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Initialization:" << endmsg;
    msg() << MSG::DEBUG	  << "RazorCut: " << m_RazorCut << endmsg;
  }  

  m_accepted=0;
  m_rejected=0;
  m_errors=0;

  return HLT::OK;
}

// ----------------------------------------------------------------------
HLT::ErrorCode TrigEFRazorAllTE::hltFinalize(){
  // ----------------------------------------------------------------------

  msg() << MSG::INFO << "in finalize()" << endmsg;
  msg() << MSG::INFO << "Events accepted/rejected/errors:  "<< m_accepted <<" / "<<m_rejected<< " / "<< m_errors<< endmsg;
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

  msg() << MSG::DEBUG << "***  Executing this TrigJetHypo : " << name() << endmsg;


  // Let's get the collections from the input TEs //////////////////////////////////////

  // Check size of vector of TEs
  if (tes_in.size() < 1) {
    msg() << MSG::WARNING << "No TriggerElements provided -> do nothing" << endmsg;
    return HLT::MISSING_FEATURE; // since no TE are found while there should be something (why would the chain start otherwise?)
  }
  else if (tes_in.size() < 2) {
    msg() << MSG::WARNING << "Less than 2 TriggerElements provided -> do nothing" << endmsg;
    return HLT::MISSING_FEATURE; // since no TE are found while there should be something (why would the chain start otherwise?)
  }


  // Check size of individual TEs
  if(tes_in[0].size() == 0) {
    msg() << MSG::WARNING << "No MET TE found" << endmsg;
    return HLT::MISSING_FEATURE; // put here not OK but something that is not aborting further execution
  }
  if(tes_in[1].size() == 0) {
    msg() << MSG::WARNING << "No jet TE found" << endmsg;
    return HLT::MISSING_FEATURE; // put here not OK but something that is not aborting further execution
  }

  beforeExecMonitors().ignore();

  HLT::TEVec allTEs;

  // Retrieve MET from tes_in[0]
  // std::vector<const TrigMissingET*> vectorMissingET;
  std::vector<const xAOD::TrigMissingET*> vectorMissingET;
  HLT::ErrorCode statMET = getFeatures(tes_in[0].front(), vectorMissingET );
  if(statMET != HLT::OK ) {
    msg() << MSG::WARNING << " Failed to get vectorMissingETs " << endmsg;
    return HLT::OK;
  }
  if(vectorMissingET.size()==0){
    msg() << MSG::WARNING << " Failed to get vectorMissingETs " << endmsg;
    return HLT::MISSING_FEATURE;
  }

  if ( (tes_in.size()>0) &&  (tes_in[0].size()>0) ) allTEs.push_back( tes_in[0][0] );
    
  // const TrigMissingET* efmet = vectorMissingET.front();
  const xAOD::TrigMissingET* efmet = vectorMissingET.front();

  // Retrieve Hemisphere Jets from tes_in[1]
  const xAOD::JetContainer* outJets(0);
  HLT::ErrorCode statJets = getFeature(tes_in[1].front(), outJets, "TrigHLTJetHemisphereRec");
  if( statJets!=HLT::OK || outJets==0 ){
    msg() << MSG::WARNING << " Got no JetCollections associated to the TE! " << endmsg;
    m_errors++;
    return HLT::OK;
  }
  if ( (tes_in.size()>0) && (tes_in[1].size()>0) ) allTEs.push_back( tes_in[1][0] );

  // Got all the input collections /////////////////////////////////////////////////////


  // Creation of internal inputs and some sanity checks... ////////////////////////////

  // TVector3 MET(efmet->ex()/1000., efmet->ey()/1000., 0.0);

  // JetCollection -> jetcollection_t
  std::vector<const xAOD::Jet*> theJets(outJets->begin(), outJets->end());

  //check size of JetCollection not empty
  if( theJets.size() == 0){
    msg()<< MSG::WARNING << " Size of JetCollection is 0! " << endmsg;
    m_errors++;
    if (m_timersvc) m_timers[0]->stop();
    return HLT::OK;
  }

  if( theJets.size() != 2){
    msg()<< MSG::WARNING << " Size of JetCollection is 2! - Razor variables require exactly two megajets" << endmsg;
    m_errors++;
    if (m_timersvc) m_timers[0]->stop();
    return HLT::OK;
  }

  if( theJets.at(0)->jetP4().E()==0. || theJets.at(1)->jetP4().E()==0.  ){
    msg()<< MSG::WARNING << " Energy-less hemisphere. Something's wrong" << endmsg;
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
  
  // trying to go for speed here. Switched from TLVs to arrays
  // Boosting is done now by writing out the analytical result 
  // in terms of the original four vector components

  m_jet[0][0] = theJets.at(0)->jetP4().Px()/Units::GeV;
  m_jet[0][1] = theJets.at(0)->jetP4().Py()/Units::GeV;
  m_jet[0][2] = theJets.at(0)->jetP4().Pz()/Units::GeV;
  m_jet[0][3] = theJets.at(0)->jetP4().E() /Units::GeV;
  m_jet[1][0] = theJets.at(1)->jetP4().Px()/Units::GeV;
  m_jet[1][1] = theJets.at(1)->jetP4().Py()/Units::GeV;
  m_jet[1][2] = theJets.at(1)->jetP4().Pz()/Units::GeV;
  m_jet[1][3] = theJets.at(1)->jetP4().E() /Units::GeV;

  m_met[0]    = efmet->ex()/Units::GeV;
  m_met[1]    = efmet->ey()/Units::GeV;

  // Fill for monitoring
  m_passedJetCuts_jetphi_0 = theJets.at(0)->jetP4().Phi();
  m_passedJetCuts_jeteta_0 = theJets.at(0)->jetP4().Eta();
  m_passedJetCuts_jetet_0  = theJets.at(0)->jetP4().Et()/Units::GeV;

  m_passedJetCuts_jetphi_1 = theJets.at(1)->jetP4().Phi();
  m_passedJetCuts_jeteta_1 = theJets.at(1)->jetP4().Eta();
  m_passedJetCuts_jetet_1  = theJets.at(1)->jetP4().Et()/Units::GeV;


  // jet masses squared 
  m_jet_m2[0] = m_jet[0][3]*m_jet[0][3]-m_jet[0][0]*m_jet[0][0]-m_jet[0][1]*m_jet[0][1]-m_jet[0][2]*m_jet[0][2];
  m_jet_m2[1] = m_jet[1][3]*m_jet[1][3]-m_jet[1][0]*m_jet[1][0]-m_jet[1][1]*m_jet[1][1]-m_jet[1][2]*m_jet[1][2];

  // here is the variable calculations - will only explicitly calculate
  // m_shatR and m_gaminvRp1 for speed
  
  // px and py for the di-hemisphere system
  m_di_hemi_pt[0] = m_jet[0][0]+m_jet[1][0];
  m_di_hemi_pt[1] = m_jet[0][1]+m_jet[1][1];
  m_di_hemi_E  = m_jet[0][3]+m_jet[1][3];  // sum E of di-hemisphere system
  m_di_hemi_pz = m_jet[0][2]+m_jet[1][2];  // sum pZ of di-hemisphere system
  m_di_hemi_mz = m_di_hemi_E*m_di_hemi_E - m_di_hemi_pz*m_di_hemi_pz;
  m_di_hemi_m2 = m_di_hemi_mz - m_di_hemi_pt[0]*m_di_hemi_pt[0]-m_di_hemi_pt[1]*m_di_hemi_pt[1];
  m_di_hemi_mz = sqrt(m_di_hemi_mz);

  // A boosting procedure to get into the R frame
  m_Ez2 = m_jet[0][3]*m_jet[1][3]-m_jet[0][2]*m_jet[1][2];
  if(m_di_hemi_mz){
    m_jet[0][2] = (m_jet[1][3]*m_jet[0][2]-m_jet[0][3]*m_jet[1][2])/m_di_hemi_mz;
    m_jet[1][2] = -m_jet[0][2];
    m_jet[0][3] = (m_Ez2 + m_jet_m2[0] + m_jet[0][0]*m_jet[0][0] + m_jet[0][1]*m_jet[0][1])/m_di_hemi_mz;
    m_jet[1][3] = (m_Ez2 + m_jet_m2[1] + m_jet[1][0]*m_jet[1][0] + m_jet[1][1]*m_jet[1][1])/m_di_hemi_mz;
  } else {
    m_jet[0][2] = 0.;
    m_jet[1][2] = 0.;
    m_jet[0][3] = 0.;
    m_jet[1][3] = 0.;
  }

  if(m_jet_m2[0]<0.) m_jet_m2[0] = -1.*m_jet_m2[0];
  if(m_jet_m2[1]<0.) m_jet_m2[1] = -1.*m_jet_m2[1];

  if( m_met[0]*m_met[0]+m_met[1]*m_met[1]+m_di_hemi_m2-4.*sqrt(m_jet_m2[0]*m_jet_m2[1]) >= 0. ){
    m_Etot = m_di_hemi_mz + sqrt(m_met[0]*m_met[0]+m_met[1]*m_met[1]+m_di_hemi_m2-4.*sqrt(m_jet_m2[0]*m_jet_m2[1]));
  } else {
    m_Etot = m_di_hemi_mz;
  }
  m_di_hemi_pt[0] += m_met[0];
  m_di_hemi_pt[1] += m_met[1];
  m_Ptot2 = m_di_hemi_pt[0]*m_di_hemi_pt[0] + m_di_hemi_pt[1]*m_di_hemi_pt[1];

  ////////////////////////////////////
  m_shatR = sqrt(m_Etot*m_Etot - m_Ptot2);
  ////////////////////////////////////

  if(m_Etot && m_Ptot2 && m_Ptot2!=m_Etot*m_Etot){
    m_gamma = 1./sqrt(1. - m_Ptot2/m_Etot/m_Etot);
  } else {
    m_gamma = 0.;
  }

  if(m_Etot){
    m_beta[0] = m_di_hemi_pt[0]/m_Etot;
    m_beta[1] = m_di_hemi_pt[1]/m_Etot;
  } else {
    m_beta[0] = 0.;
    m_beta[1] = 0.;
  }
  if( (m_gamma+1) ){
    m_pdotbeta[0] = (m_beta[0]*m_jet[0][0]+m_beta[1]*m_jet[0][1])*m_gamma/(m_gamma+1) - m_jet[0][3];
    m_pdotbeta[1] = (m_beta[0]*m_jet[1][0]+m_beta[1]*m_jet[1][1])*m_gamma/(m_gamma+1) - m_jet[1][3];
  } else {
    m_pdotbeta[0] = 0.;
    m_pdotbeta[1] = 0.;
  }
  m_jet[0][0] = m_jet[0][0] + m_gamma*m_beta[0]*m_pdotbeta[0];
  m_jet[0][1] = m_jet[0][1] + m_gamma*m_beta[1]*m_pdotbeta[0];
  m_jet[1][0] = m_jet[1][0] + m_gamma*m_beta[0]*m_pdotbeta[1];
  m_jet[1][1] = m_jet[1][1] + m_gamma*m_beta[1]*m_pdotbeta[1];

  // double p[2];
  m_p[0] = sqrt(m_jet[0][0]*m_jet[0][0]+m_jet[0][1]*m_jet[0][1]+m_jet[0][2]*m_jet[0][2]);
  m_p[1] = sqrt(m_jet[1][0]*m_jet[1][0]+m_jet[1][1]*m_jet[1][1]+m_jet[1][2]*m_jet[1][2]);
  m_dot = m_jet[0][0]*m_jet[1][0]+m_jet[0][1]*m_jet[1][1]+m_jet[0][2]*m_jet[1][2];
  
  ////////////////////////////////////
  if((m_p[0]+m_p[1])){
    m_gaminvRp1 = sqrt(2.*(m_p[0]*m_p[1]+m_dot))/(m_p[0]+m_p[1]);
  } else {    
    m_gaminvRp1 = 0.;
  }
  ////////////////////////////////////

  // Define "razor" cut as m_gaminvRp1 * m_shatR
  // This is "Pi" according to the talks. 

  m_Razor = m_gaminvRp1 * m_shatR;

  // Trigger cuts on this "razor" cut value

  if (m_Razor > m_RazorCut) {
    m_Razor_passedRazor = m_Razor;

    pass = true;
    m_accepted++;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << ">>>>> TRIGGER PASSED <<<<<       Razor: " << m_Razor << " PASSED Razor cut of " << m_RazorCut << endmsg;
    m_cutCounter = 2;
  }
  else {
    pass = false;
    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << ">>>>> TRIGGER FAILED <<<<<       Razor: " << m_Razor << " FAILED Razor cut of " << m_RazorCut << endmsg;
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
