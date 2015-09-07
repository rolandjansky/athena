/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <cmath>

#include "TrigMuonHypo/TrigMuonEFCombinerDiMuonMassHypo.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigMuonEFCombinerDiMuonMassHypo::TrigMuonEFCombinerDiMuonMassHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator)
{
  declareProperty("AcceptAll",        m_acceptAll       = true);
  declareProperty("massThresLow",     m_massThresLow    = 2.8);
  declareProperty("massThresHigh",    m_massThresHigh   = 3.4);
  declareProperty("signRequirement",  m_signRequirement = -1);

  declareMonitoredStdContainer("nMuons",        m_mon_nMuons);
  declareMonitoredStdContainer("chargeMult",    m_mon_chargeMult);
  declareMonitoredStdContainer("invMassWide",   m_mon_invMass);
  declareMonitoredStdContainer("invMassNarrow", m_mon_invMass);
  declareMonitoredStdContainer("muonPt",        m_mon_muonPt);
  declareMonitoredStdContainer("muonEta",       m_mon_muonEta);

  m_storeGate=0;
}

TrigMuonEFCombinerDiMuonMassHypo::~TrigMuonEFCombinerDiMuonMassHypo() {
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonEFCombinerDiMuonMassHypo::hltInitialize()
{
  msg() << MSG::INFO << "in hltInitialize : " << name() << endreq;
   
  if( m_acceptAll ) {
    msg() << MSG::INFO << "Accepting all the events without cut!" << endreq;
  }

  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonEFCombinerDiMuonMassHypo::hltFinalize()
{
  msg() << MSG::INFO << "in hltFinalize : " << name() << endreq;
  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonEFCombinerDiMuonMassHypo::hltBeginRun()
{
  msg() << MSG::INFO << "in hltBeginRun : " << name() << endreq;

  msg() << MSG::INFO << "sign requirement flag : " << m_signRequirement << endreq;
  msg() << MSG::INFO << "mass threshold low    : " << m_massThresLow << endreq;
  msg() << MSG::INFO << "mass threshold high   : " << m_massThresHigh << endreq;

  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonEFCombinerDiMuonMassHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{
  m_storeGate = store();

  bool debug = msgLvl() <= MSG::DEBUG;

  if(debug) msg() << MSG::DEBUG << "in hltExecute : " << name() << endreq;

  // reset monitoring variables
  m_mon_nMuons.clear();
  m_mon_chargeMult.clear();
  m_mon_invMass.clear();
  m_mon_muonPt.clear();
  m_mon_muonEta.clear();

  // if acceptAll
  if( m_acceptAll ) {
    pass = true;
    if(debug) msg() << MSG::DEBUG << "AcceptAll property is set: taking all the events" << endreq;
    return HLT::OK;
  }

  if(debug) msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;

  // get TrigMuonEFInfoContainer
  const xAOD::MuonContainer* muonContainer;
  if(getFeature(outputTE, muonContainer)!=HLT::OK) {
    if (debug) msg() << MSG::DEBUG << "No xAOD::MuonContainer found." << endreq;
    return HLT::MISSING_FEATURE;
  } 
  else{
    if (!muonContainer) { 
      if (debug) msg() << MSG::DEBUG << "null xAOD::MuonContainer Feature found" << endreq; 
      return HLT::MISSING_FEATURE; 
    } 
  }
  if (debug) msg() << MSG::DEBUG << "xAOD::MuonContainer found with size " << muonContainer->size() << endreq;
   
  // collect all CB muon
  std::vector<double> vCBPt;
  std::vector<double> vCBEta;
  std::vector<double> vCBPhi;
  std::vector<double> vCBCharge;


  // loop on the muons within the RoI
  for (unsigned int j=0; j<muonContainer->size(); j++ ) {

    if (debug) msg() << MSG::DEBUG << "++ Looking at xAOD:Muon" << j << endreq;
    const xAOD::Muon* muon = muonContainer->at(j);
    if (!muon) {
      if (debug) msg() << MSG::DEBUG << "No xAOD::Muon found." << endreq;
      continue;
    }
    if (! muon->primaryTrackParticle()) {
      if (debug) msg() << MSG::DEBUG << "No track found." << endreq;
      continue;
    }


    const xAOD::TrackParticle* tr = muon->trackParticle(xAOD::Muon::CombinedTrackParticle);

    if (!tr) {
      if (debug) msg() << MSG::DEBUG << "No CombinedTrackParticle found." << endreq;
      continue;
    }
    if (debug) msg() << MSG::DEBUG << "CB track: pt/Charge/chi2=" << (*tr).pt()/CLHEP::GeV << "/" << tr->charge() << "/" << tr->chiSquared() << endreq;

    double pt    = tr->pt()/CLHEP::GeV;
    double eta   = tr->eta();
    double phi   = tr->phi();
    double charge= tr->charge();

    vCBPt.push_back(pt);
    vCBEta.push_back(eta);
    vCBPhi.push_back(phi);
    vCBCharge.push_back(charge);
	 
  }

  // n muons >=2 
  int n_cb_muons = vCBPt.size();
  m_mon_nMuons.push_back(n_cb_muons); // monitoring
  if( n_cb_muons < 2 ) {
    pass = false;
    if(debug) msg() << MSG::DEBUG << "Event rejected as: nr of muons less than two = " << n_cb_muons << endreq;
    return HLT::OK;
  }

  // make mass cut
  if(debug) msg() << MSG::DEBUG << "mass cut on all combinations..." << endreq;
  int n_pair_passed = 0;
  for(int i=0; i<n_cb_muons-1; i++) {
    double pt1     = vCBPt[i];
    double eta1    = vCBEta[i];
    double phi1    = vCBPhi[i];
    double charge1 = vCBCharge[i];
    if(debug) msg() << MSG::DEBUG << "pt1/eta1/phi1/charge1=" << pt1 << "/" << eta1 << "/" << phi1 << "/" << charge1 << endreq;
    for(int j=i+1; j<n_cb_muons; j++) {
      double pt2     = vCBPt[j];
      double eta2    = vCBEta[j];
      double phi2    = vCBPhi[j];
      double charge2 = vCBCharge[j];
      if(debug) msg() << MSG::DEBUG << "   pt2/eta2/phi2/charge2=" << pt2 << "/" << eta2 << "/" << phi2 << "/" << charge2 << endreq;
      m_mon_chargeMult.push_back(charge1*charge2); // monitoring
      if( (m_signRequirement== 1) && (charge1*charge2 < 0) ) { // same sign
	if(debug) msg() << MSG::DEBUG << "   ... not same sign" << endreq;
	continue;
      }
      if( (m_signRequirement==-1) && (charge1*charge2 > 0) ) { // opposite sign
	if(debug) msg() << MSG::DEBUG << "   ... not opposite sign" << endreq;
	continue;
      }
      const double MUON_MASS = 0.1057;
      double mass = invMass(MUON_MASS,pt1,eta1,phi1,MUON_MASS,pt2,eta2,phi2);
      m_mon_invMass.push_back(mass); // monitoring
      if(debug) msg() << MSG::DEBUG << "      mass=" << mass << endreq;
      if( m_massThresLow < mass && mass < m_massThresHigh ) {
	n_pair_passed++;
	if(debug) msg() << MSG::DEBUG << "      -> mass cut passed" << endreq;
	// monitoring
	m_mon_muonPt.push_back(pt1);         m_mon_muonPt.push_back(pt2);
	m_mon_muonEta.push_back(eta1);       m_mon_muonEta.push_back(eta2);
      }
    }
  }

  // decision: is Jpsi there?
  if( n_pair_passed > 0 ) {
    pass = true;
    if(debug) msg() << MSG::DEBUG << "Event passed." << endreq;
  }
  else {
    pass = false;
    if(debug) msg() << MSG::DEBUG << "Event rejected as: no di-muon pair passed mass cut." << endreq;
  }

  return HLT::OK;
}  

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigMuonEFCombinerDiMuonMassHypo::invMass(double m1, double pt1, double eta1, double phi1,
						 double m2, double pt2, double eta2, double phi2)
{
  const double ZERO_LIMIT = 1e-12;

  double theta1 = 2*atan2((double)exp(-eta1),1.);
  double theta2 = 2*atan2((double)exp(-eta2),1.);

  double fpt1   = fabs(pt1);
  double fpt2   = fabs(pt2);

  double px1    = fpt1*cos(phi1);
  double py1    = fpt1*sin(phi1);
  double pz1    = fpt1/tan(theta1);
  double  e1    = sqrt(px1*px1+py1*py1+pz1*pz1+m1*m1);

  double px2    = fpt2*cos(phi2);
  double py2    = fpt2*sin(phi2);
  double pz2    = fpt2/tan(theta2); 
  double  e2    = sqrt(px2*px2+py2*py2+pz2*pz2+m2*m2); 

  double pxsum  = px1 + px2;
  double pysum  = py1 + py2;
  double pzsum  = pz1 + pz2;
  double esum   =  e1 +  e2; 

  double mass  = 0;
  double mass2 = esum*esum - pxsum*pxsum - pysum*pysum - pzsum*pzsum;
  if( mass2 > ZERO_LIMIT ) mass = sqrt(mass2);

  return mass;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------
