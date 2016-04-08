/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <set>
#include <cmath>

#include "TrigMuonHypo/TrigMuonEFCombinerDiMuonMassPtImpactsHypo.h"

#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

class ISvcLocator;

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------


const double TrigMuonEFCombinerDiMuonMassPtImpactsHypo::MUMASS = 0.1056583715; // in GeV
const double TrigMuonEFCombinerDiMuonMassPtImpactsHypo::MASS_ZERO_LIMIT = 1e-12;
  

TrigMuonEFCombinerDiMuonMassPtImpactsHypo::TrigMuonEFCombinerDiMuonMassPtImpactsHypo(const std::string & name, ISvcLocator* pSvcLocator):
  // HLT::HypoAlgo(name, pSvcLocator)
  HLT::AllTEAlgo(name, pSvcLocator)
{
  declareProperty("AcceptAll",        m_acceptAll       = false);         //pvn true -> false
  declareProperty("massThresLow",     m_massThresLow    = -1);           //pvn 2.8->9.7
  declareProperty("massThresHigh",    m_massThresHigh   = -1);           //pvn 3.4->30.7
  declareProperty("pairptThresLow",      m_pairptThresLow     = -1);            //pvn
  declareProperty("pairptThresHigh",      m_pairptThresHigh     = -1);            //pvn
  declareProperty("deltaZThres",      m_deltaZThres     = -1);            //pvn
  declareProperty("deltaPhiThresLow",    m_deltaPhiThresLow   = -1);
  declareProperty("deltaPhiThresHigh",    m_deltaPhiThresHigh   = -1);

  declareMonitoredStdContainer("nMuons",        m_mon_nMuons);
  declareMonitoredStdContainer("invMassWide",   m_mon_invMass);
  declareMonitoredStdContainer("dimuonPt",      m_mon_dimuonPt);
  declareMonitoredStdContainer("muonDeltaZ",    m_mon_muonDeltaZ);
  declareMonitoredStdContainer("muonDeltaPhi",  m_mon_muonDeltaPhi);

  m_storeGate=0;
}

TrigMuonEFCombinerDiMuonMassPtImpactsHypo::~TrigMuonEFCombinerDiMuonMassPtImpactsHypo() {
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonEFCombinerDiMuonMassPtImpactsHypo::hltInitialize()
{
  msg() << MSG::INFO << "in hltInitialize : " << name() << endreq;
   
  if( m_acceptAll ) {
    msg() << MSG::INFO << "Accepting all the events without cut!" << endreq;
  }

  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonEFCombinerDiMuonMassPtImpactsHypo::hltFinalize()
{
  msg() << MSG::INFO << "in hltFinalize : " << name() << endreq;
  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonEFCombinerDiMuonMassPtImpactsHypo::hltBeginRun()
{
  msg() << MSG::INFO << "in hltBeginRun : " << name() << endreq;

  msg() << MSG::INFO << "AcceptAll : " << m_acceptAll << endreq;
  msg() << MSG::INFO << "mass threshold low(GeV)    : " << m_massThresLow << endreq;
  msg() << MSG::INFO << "mass threshold high(GeV)   : " << m_massThresHigh << endreq;
  msg() << MSG::INFO << "pairpt threshold low(GeV)    : " << m_pairptThresLow << endreq;
  msg() << MSG::INFO << "pairpt threshold high(GeV)   : " << m_pairptThresHigh << endreq;
  msg() << MSG::INFO << "delta Z threshold(mm)   : " << m_deltaZThres << endreq;
  msg() << MSG::INFO << "delta Phi threshold low(Rad)    : " << m_deltaPhiThresLow << endreq;
  msg() << MSG::INFO << "delta Phi threshold high(Rad)   : " << m_deltaPhiThresHigh << endreq;

  return HLT::OK;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

HLT::ErrorCode TrigMuonEFCombinerDiMuonMassPtImpactsHypo::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out)
{
  m_storeGate = store();

  bool debug = msgLvl() <= MSG::DEBUG;

  if(debug) msg() << MSG::DEBUG << "in hltExecute : " << name() << endreq;

  // reset monitoring variables
  bool doMonitor = true;
  if(doMonitor){
    beforeExecMonitors().ignore();
    m_mon_nMuons.clear();
    m_mon_invMass.clear();
    m_mon_muonDeltaZ.clear();
    m_mon_muonDeltaPhi.clear();
    m_mon_dimuonPt.clear();
  }
  
  // ---
  // collect all TEs and create output TEs with ActiveState=true
  // ---
  
  std::vector<HLT::TriggerElement*> vec_allTEs;
  std::vector<HLT::TriggerElement*> vec_outputTEs;
  
  unsigned int tes_in_size = tes_in.size();
  
  unsigned int i_te;
  unsigned int j_te;
  
  if(debug) msg() << MSG::DEBUG << "tes_in size=" << tes_in_size << endreq;
  for(unsigned int i_vec=0; i_vec<tes_in_size; i_vec++) {
    unsigned int n_te = tes_in[i_vec].size();
    if(debug) msg() << MSG::DEBUG << "i_vec=" << i_vec << " : n TEs=" << n_te << endreq;
    for(i_te=0; i_te<n_te; i_te++) {
      HLT::TriggerElement* te = tes_in[i_vec][i_te];
      if( debug ) {
	std::string label;
	TrigConf::HLTTriggerElement::getLabel (te->getId(), label );
	if(debug) msg() << MSG::DEBUG << "input TE ID(): " << te->getId() << ", Label=" << label << endreq;
      }      
      bool alreadyThere = false;
      for(unsigned int j_te=0; j_te<vec_allTEs.size(); j_te++) {
	if( vec_allTEs[j_te] == te ) {
	  if(debug) msg() << MSG::DEBUG << "at i_vec=" << i_vec << ", i_te=" << i_te <<
		      ": same TE already there at j_te=" << j_te << ", so, skip this TE." << endreq;
	  alreadyThere = true;
	  break;
	}
      }
      if( ! alreadyThere ) vec_allTEs.push_back(te);
    }
  }
  
  unsigned int n_allTEs = vec_allTEs.size(); 
  if(debug) msg() << MSG::DEBUG << "size of vec_allTEs=" << n_allTEs << endreq;
  for(i_te=0; i_te<n_allTEs; i_te++) {
    HLT::TEVec tes;
    tes.push_back(vec_allTEs[i_te]);
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(tes, type_out);
    if(debug) {
      std::string label;
      TrigConf::HLTTriggerElement::getLabel (outputTE->getId(), label);
      msg() << MSG::DEBUG << "creating outputTE ID(): " << outputTE->getId() << ", Label=" << label << endreq;
    }
    outputTE->setActiveState(true);
    vec_outputTEs.push_back(outputTE);
  }

  if( n_allTEs <= 1 ) {
    if(debug) msg() << MSG::DEBUG << "nr of TEs <= 1, exitting with all TEs active" << endreq;
    if(doMonitor)afterExecMonitors().ignore();
    return HLT::OK;
  }
  
  
  bool errorWhenGettingELs = false;
  std::vector<std::vector<const xAOD::MuonContainer*> > EFMuonELvec; 
  for(i_te=0; i_te<n_allTEs; i_te++) {
    std::vector<const xAOD::MuonContainer*> EFMuonEL;
    HLT::ErrorCode isMuExtrOK = getFeatures(vec_allTEs[i_te], EFMuonEL);
    if( isMuExtrOK != HLT::OK ) {
      errorWhenGettingELs = true;
      msg() << MSG::WARNING << "i_te=" << i_te << ": fails to find EL for: EFMuon" << endreq;
      continue;
    }
    if( EFMuonEL.empty() ) {
      errorWhenGettingELs = true;
      msg() << MSG::WARNING << "i_te=" << i_te << ": EL not valid for: EFMuon" << endreq;
      continue;
    }
    if(debug) msg() << MSG::DEBUG << "EFMuonEL is OK for i_te=" << i_te << " size of EFMuonEL is " << EFMuonEL.size() << endreq;
    EFMuonELvec.push_back(EFMuonEL);
  }
  
  if( errorWhenGettingELs ) {
    msg() << MSG::WARNING << "error when getting ELs. exitting with all TEs active..." << endreq;
    if(doMonitor)afterExecMonitors().ignore();
    return HLT::ErrorCode(HLT::Action::CONTINUE,HLT::Reason::NAV_ERROR);
  }
  
  int n_cb_muons = 0;

  std::set<unsigned int> ok_TEs;
  int n_pair_passed = 0;
  for(i_te=0; i_te<n_allTEs; i_te++) {
    std::vector<const xAOD::MuonContainer*> VecMuEFCont1 = EFMuonELvec[i_te];
    for (unsigned int ivmu1=0; ivmu1<VecMuEFCont1.size(); ivmu1++){
      if (debug) msg() << MSG::DEBUG << "Element1 ivmu1=" << ivmu1 << " of vector of TrigMuonEFInfo containers" << endreq;
      const xAOD::MuonContainer* trigMuon1 = VecMuEFCont1[ivmu1];
      if( !trigMuon1 ){
	if (debug) msg() << MSG::DEBUG << "failed: trigMuon1 is empty" << endreq;
	continue;
      }
      for (unsigned int imu1 = 0 ; imu1 < trigMuon1->size() ; imu1++){
	if (debug) msg() << MSG::DEBUG << "muon1 : imu1=" << imu1 << endreq;
	const xAOD::Muon* muon1 = trigMuon1->at(imu1);
	if( ! muon1 ){
	  if (debug) msg() << MSG::DEBUG << "failed: muon1 is empty" << endreq;
	  continue;
	}
	if( ! muon1->primaryTrackParticle() ){
	  if (debug) msg() << MSG::DEBUG << "failed: muon1->primaryTrackParticle() is empty" << endreq;
	  continue;
	}
	const xAOD::TrackParticle* track1 = muon1->trackParticle(xAOD::Muon::CombinedTrackParticle);
	if( ! track1 ){
	  if (debug) msg() << MSG::DEBUG << "failed: muon1->trackParticle(xAOD::Muon::CombinedTrackParticle) is empty" << endreq;
	  continue;
	}
	double trk1_pt = track1->pt() / CLHEP::GeV;
	double trk1_eta = track1->eta();
	double trk1_phi = track1->phi();
	double trk1_z0  = track1->z0();
	n_cb_muons++;
	for(j_te=i_te; j_te<n_allTEs; j_te++) {
	  if(i_te == j_te)continue;
	  std::vector<const xAOD::MuonContainer*> VecMuEFCont2 = EFMuonELvec[j_te];
	  for (unsigned int ivmu2=0; ivmu2<VecMuEFCont2.size(); ivmu2++){
	    if (debug) msg() << MSG::DEBUG << "Element2 ivmu2=" << ivmu2 << " of vector of TrigMuonEFInfo containers" << endreq;
	    const xAOD::MuonContainer* trigMuon2 = VecMuEFCont2[ivmu2];
	    if( !trigMuon2 ){
	      if (debug) msg() << MSG::DEBUG << "failed: trigMuon2 is empty" << endreq;
	      continue;
	    }
	    for (unsigned int imu2 = 0 ; imu2 < trigMuon2->size() ; imu2++){
	      if (debug) msg() << MSG::DEBUG << "muon2 : imu2=" << imu2 << endreq;
	      const xAOD::Muon* muon2 = trigMuon2->at(imu2);
	      if( ! muon2 ){
		if (debug) msg() << MSG::DEBUG << "failed: muon2 is empty" << endreq;
		continue;
	      }
	      if( ! muon2->primaryTrackParticle() ){
		if (debug) msg() << MSG::DEBUG << "failed: muon2->primaryTrackParticle() is empty" << endreq;
		continue;
	      }
	      const xAOD::TrackParticle* track2 = muon2->trackParticle(xAOD::Muon::CombinedTrackParticle);
	      if( ! track2 ){
		if (debug) msg() << MSG::DEBUG << "failed: muon2->trackParticle(xAOD::Muon::CombinedTrackParticle) is empty" << endreq;
		continue;
	      }
	      double trk2_pt = track2->pt() / CLHEP::GeV;
	      double trk2_eta = track2->eta();
	      double trk2_phi = track2->phi();
	      double trk2_z0  = track2->z0();

	      if(debug) msg() << MSG::DEBUG << "Dimuon candidate" << endreq;
	      if(debug) msg() << MSG::DEBUG << "trk1_pt=" << trk1_pt <<   " trk2_pt=" << trk2_pt << endreq;
	      if(debug) msg() << MSG::DEBUG << "trk1_eta=" << trk1_eta << " trk2_eta=" << trk2_eta << endreq;
	      if(debug) msg() << MSG::DEBUG << "trk1_phi=" << trk1_phi << " trk2_phi=" << trk2_phi << endreq;
	      if(debug) msg() << MSG::DEBUG << "trk1_z0=" << trk1_z0 <<   " trk2_z0=" << trk2_z0 << endreq;


	      double deltaPhi = trk1_phi - trk2_phi;
	      while(deltaPhi>M_PI)  deltaPhi -= 2.0 * M_PI;
	      while(deltaPhi<-M_PI) deltaPhi += 2.0 * M_PI;
	      deltaPhi = fabs( deltaPhi );
	      double deltaZ0 = fabs(trk1_z0 - trk2_z0);
	      double ptsum = pairPt(trk1_pt,trk1_phi,trk2_pt,trk2_phi);
	      double mass = invMass(trk1_pt,trk1_eta,trk1_phi,trk2_pt,trk2_eta,trk2_phi);
	      
	      if( fabs(trk1_pt - trk2_pt)<0.001 &&
		  fabs(trk1_eta - trk2_eta)<0.001 &&
		  deltaPhi<0.001 &&
		  deltaZ0<0.001 ){
		if (debug) msg() << MSG::DEBUG << "failed: muon1 = muon2" << endreq;
		continue;
	      }
	      	      
	      if(debug) msg() << MSG::DEBUG << "      delta z0=" << deltaZ0 << endreq;
	      if(debug) msg() << MSG::DEBUG << "      delta phi=" << deltaPhi << endreq;
	      if(debug) msg() << MSG::DEBUG << "      dimuon pt=" << ptsum << endreq;
	      if(debug) msg() << MSG::DEBUG << "      dimuon mass=" << mass << endreq;

	      if(debug) msg() << MSG::DEBUG << "      delta Z threshold(mm)   : " << m_deltaZThres << endreq;
	      if(debug) msg() << MSG::DEBUG << "      delta Phi threshold low(Rad)    : " << m_deltaPhiThresLow << endreq;
	      if(debug) msg() << MSG::DEBUG << "      delta Phi threshold high(Rad)   : " << m_deltaPhiThresHigh << endreq;
	      if(debug) msg() << MSG::DEBUG << "      pairpt threshold low(GeV)    : " << m_pairptThresLow << endreq;
	      if(debug) msg() << MSG::DEBUG << "      pairpt threshold high(GeV)   : " << m_pairptThresHigh << endreq;
	      if(debug) msg() << MSG::DEBUG << "      mass threshold low(GeV)    : " << m_massThresLow << endreq;
	      if(debug) msg() << MSG::DEBUG << "      mass threshold high(GeV)   : " << m_massThresHigh << endreq;
	      
	      /////////////////////////////////////////////////////////////////
	      if(doMonitor)m_mon_muonDeltaZ.push_back(deltaZ0);
	      if( m_deltaZThres >= 0 && deltaZ0 > m_deltaZThres ) {
		if(debug) msg() << MSG::DEBUG << "      -> delta z0 cut failed" << endreq;
		continue;
	      }
	      if(debug) msg() << MSG::DEBUG << "      -> delta z0 cut passed" << endreq;
	      /////////////////////////////////////////////////////////////////	      
	      if(doMonitor)m_mon_muonDeltaPhi.push_back(deltaPhi);
	      if( m_deltaPhiThresLow >= 0 && deltaPhi < m_deltaPhiThresLow ) {
		if(debug) msg() << MSG::DEBUG << "      -> delta phi Low  cut failed" << endreq;
		continue;
	      }
	      if( m_deltaPhiThresHigh >= 0 && deltaPhi > m_deltaPhiThresHigh ) {
		if(debug) msg() << MSG::DEBUG << "      -> delta phi High cut failed" << endreq;
		continue;
	      }
	      if(debug) msg() << MSG::DEBUG << "      -> delta phi cut passed" << endreq;
	      /////////////////////////////////////////////////////////////////	      
	      if(doMonitor)m_mon_dimuonPt.push_back(ptsum);
	      if( m_pairptThresLow >= 0 && ptsum < m_pairptThresLow ) {
		if(debug) msg() << MSG::DEBUG << "      -> pair pt Low cut failed" << endreq;
		continue;
	      }
	      if( m_pairptThresHigh >= 0 && ptsum > m_pairptThresHigh ) {
		if(debug) msg() << MSG::DEBUG << "      -> pair pT High cut failed" << endreq;
		continue;
	      }
	      if(debug) msg() << MSG::DEBUG << "      -> pair pT cut passed" << endreq;
	      /////////////////////////////////////////////////////////////////	      
	      if(doMonitor)m_mon_invMass.push_back(mass);
	      if( m_massThresLow >= 0 && mass < m_massThresLow ){
		if(debug) msg() << MSG::DEBUG << "      -> dimuon mass Low cut failed" << endreq;
		continue;
	      }
	      if(debug) msg() << MSG::DEBUG << "      -> dimuon mass Low passed" << endreq;
	      if( m_massThresHigh >= 0 && mass > m_massThresHigh ) {
		if(debug) msg() << MSG::DEBUG << "      -> dimuon mass High cut failed" << endreq;
		continue;
	      }
	      if(debug) msg() << MSG::DEBUG << "      -> dimuon mass High passed" << endreq;
	      /////////////////////////////////////////////////////////////////

	      if(debug) msg() << MSG::DEBUG << "      -> passing dimuon selection" << endreq;
	      
	      n_pair_passed++;
	      ok_TEs.insert(i_te);
	      ok_TEs.insert(j_te);
	    }
	  }
	}
      }
    }
  }

  if(doMonitor)m_mon_nMuons.push_back(n_cb_muons);
  
  
  if( ok_TEs.size() == 1 ) {
    msg() << MSG::WARNING << "Number of ok_TES = " << ok_TEs.size() << ". Should be 0 or >1" << endreq;
  }

  for(i_te=0; i_te<n_allTEs; i_te++) {
    if(ok_TEs.find(i_te) == ok_TEs.end()){
      vec_outputTEs[i_te]->setActiveState(false);
      if(debug) msg() << MSG::DEBUG << "TE " << i_te << " is deactivated" << endreq;
    }else{
      if(debug) msg() << MSG::DEBUG << "TE " << i_te << " is accepted" << endreq;
    }
  }

  if(doMonitor)afterExecMonitors().ignore();
  return HLT::OK;
}  

// --------------------------------------------------------------------------------

double TrigMuonEFCombinerDiMuonMassPtImpactsHypo::invMass(double pt1, double eta1, double phi1,
						          double pt2, double eta2, double phi2)
{

  double theta1 = 2*atan2((double)exp(-eta1),1.);
  double theta2 = 2*atan2((double)exp(-eta2),1.);

  double fpt1   = fabs(pt1);
  double fpt2   = fabs(pt2);

  double px1    = fpt1*cos(phi1);
  double py1    = fpt1*sin(phi1);
  double pz1    = fpt1/tan(theta1);
  double  e1    = sqrt(px1*px1+py1*py1+pz1*pz1+MUMASS*MUMASS);

  double px2    = fpt2*cos(phi2);
  double py2    = fpt2*sin(phi2);
  double pz2    = fpt2/tan(theta2); 
  double  e2    = sqrt(px2*px2+py2*py2+pz2*pz2+MUMASS*MUMASS); 

  double pxsum  = px1 + px2;
  double pysum  = py1 + py2;
  double pzsum  = pz1 + pz2;
  double esum   =  e1 +  e2; 

  double mass  = 0;
  double mass2 = esum*esum - pxsum*pxsum - pysum*pysum - pzsum*pzsum;
  if( mass2 > MASS_ZERO_LIMIT ) mass = sqrt(mass2);

  return mass;
}

// --------------------------------------------------------------------------------

double TrigMuonEFCombinerDiMuonMassPtImpactsHypo::pairPt(double pt1, double phi1, double pt2, double phi2)
{

  double fpt1   = fabs(pt1);
  double fpt2   = fabs(pt2);

  double px1    = fpt1*cos(phi1);
  double py1    = fpt1*sin(phi1);

  double px2    = fpt2*cos(phi2);
  double py2    = fpt2*sin(phi2);

  double pxsum  = px1 + px2;
  double pysum  = py1 + py2;

  double ptsum = sqrt( pxsum*pxsum + pysum*pysum );

  return ptsum;
}

// --------------------------------------------------------------------------------
