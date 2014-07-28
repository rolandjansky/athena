/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  TrigMuonCluster.cxx
*/
#include "TMath.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/ITHistSvc.h"
#include "PathResolver/PathResolver.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigSteeringEvent/PhiHelper.h"
//LVL1 ROIS
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//TIMERS
#include "TrigTimeAlgs/TrigTimer.h"
//MUON CLUSTER
#include "TrigL2LongLivedParticles/MuonCluster.h"
#include "TrigMuonEvent/TrigMuonClusterFeature.h"
//JETS
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"
//ID TRACKING
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"

using namespace std;

MuonCluster::MuonCluster(const std::string& name, ISvcLocator* svc) 
  : HLT::AllTEAlgo(name, svc), m_useCachedResult(false), m_clu_feature(0), m_cachedTE(0),
    mStoreGate("StoreGateSvc", this->name()) {

  mCluEta = 0.;
  mCluPhi = 0.;
  mCluNum = 0;
  mNumJet = -1;
  mNumTrk = -1;

  declareProperty("DeltaR",              mDeltaR=0.4,                     "radius of the muon cluster");
  declareProperty("DeltaRJet",           mDeltaRJet=0.7,                  "distance between jet and muon cluster");
  declareProperty("DeltaRTrk",           mDeltaRTrk=0.4,                  "distance between track and muon cluster");
  declareProperty("MuonCluLabel",        m_featureLabel = "MuonCluster",  "label for the MuonCluster feature in the HLT Navigation");
  declareProperty("UseID",               m_UseID = true,                  "to use or not the ID informations");
  declareProperty("IDalgo",              m_ID_algo_to_use = "STRATEGY_B", "ID tracking algorithm");
  declareProperty("PtMinID",             m_PtMinID = 5000.0,              "minimum Pt (MeV) for ID track");
  declareMonitoredVariable("RoIEta", mCluEta);
  declareMonitoredVariable("RoIPhi", mCluPhi);
  declareMonitoredVariable("NumClu", mCluNum);
  declareMonitoredVariable("NumJet", mNumJet);
  declareMonitoredVariable("NumTrk", mNumTrk);
  declareMonitoredStdContainer("RoiEta",m_RoiEta);
  declareMonitoredStdContainer("RoiPhi",m_RoiPhi);

}

MuonCluster::~MuonCluster() {
}

HLT::ErrorCode MuonCluster::hltInitialize() {

  msg() << MSG::INFO << "initialize()" << endreq;

  msg() << MSG::INFO << "Parameters for MuonCluster:" << name() << endreq;
  msg() << MSG::INFO << "DeltaR : " << mDeltaR << endreq;
  msg() << MSG::INFO << "DeltaRJet : " << mDeltaRJet << endreq;
  msg() << MSG::INFO << "DeltaRTrk : " << mDeltaRTrk << endreq;
  msg() << MSG::INFO << "MuonCluLabel : " << m_featureLabel << endreq;
  msg() << MSG::INFO << "UseID : " << m_UseID << endreq;
  msg() << MSG::INFO << "IDalgo : " << m_ID_algo_to_use << endreq;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Retrieve service StoreGateSvc" << endreq;
  }

  if (mStoreGate.retrieve().isFailure()) {
    msg() << MSG::ERROR << "Cannot retrieve service StoreGateSvc" << endreq;
    return HLT::BAD_JOB_SETUP;
  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Timers are initializated here" << endreq;
  }

  mTimers.push_back(addTimer("ClusterMuon_Initialization"));
  mTimers.push_back(addTimer("ClusterMuon_ClusterFinding"));
  mTimers.push_back(addTimer("ClusterMuon_JetsFinding"));
  mTimers.push_back(addTimer("ClusterMuon_tracksFinding"));
  mTimers.push_back(addTimer("ClusterMuon_Finalization"));

  declareMonitoredObject("Initialization", 
			 *(mTimers[ITIMER_INIT]), 
			 &TrigTimer::elapsed);
  declareMonitoredObject("ClusterFinder", *(mTimers[ITIMER_CLUSTER]), 
			 &TrigTimer::elapsed);
  declareMonitoredObject("JetsFinder", *(mTimers[ITIMER_JETS]), 
			 &TrigTimer::elapsed);
  declareMonitoredObject("tracksFinder", *(mTimers[ITIMER_TRACKS]), 
			 &TrigTimer::elapsed);
  declareMonitoredObject("Finalization", *(mTimers[ITIMER_FINAL]), 
			 &TrigTimer::elapsed);
  //Set ID Algo to use
  std::string algoId = m_ID_algo_to_use;
  if      (algoId=="IDSCAN")     m_algoId = TrigInDetTrack::IDSCANID;
  else if (algoId=="SITRACK")    m_algoId = TrigInDetTrack::SITRACKID;
  else if (algoId=="STRATEGY_A") m_algoId = TrigInDetTrack::STRATEGY_A_ID;
  else if (algoId=="STRATEGY_B") m_algoId = TrigInDetTrack::STRATEGY_B_ID;
  else if (algoId=="STRATEGY_C") m_algoId = TrigInDetTrack::STRATEGY_C_ID;
  else if (algoId=="TRTXK")      m_algoId = TrigInDetTrack::TRTXKID;
  else if (algoId=="TRTSEG")     m_algoId = TrigInDetTrack::TRTLUTID;
  else                           m_algoId = TrigInDetTrack::IDSCANID;
	

   
  msg() << MSG::INFO << "initialize() success" << endreq;

  return HLT::OK;
}

HLT::ErrorCode MuonCluster::hltBeginRun() {
  msg() << MSG::INFO << "hltBeginRun"<<  endreq;
  return HLT::OK;
}

HLT::ErrorCode MuonCluster::hltEndRun() {
  msg() << MSG::INFO << "hltEndRun"<<  endreq;
  return HLT::OK;
}

HLT::ErrorCode MuonCluster::hltFinalize() {
  msg() << MSG::INFO << "hltFinalize()" << endreq;
  return HLT::OK;
}

HLT::ErrorCode MuonCluster::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out)
{ 
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "execute()" << endreq;
  }
  
  clearEvent();

  // CACHING
  // first check whether we executed this instance before:
  // CACHING / Update implemented since tag 00-00-19 / A.C.
  // Caching disabled to avoid cross-talk between similar chains
  // i.e. chains seeded by 2MU6 or 2MU10 
  if (m_useCachedResult) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Executing this " << name() << " in cached mode" << endreq;
    }

    // Only count mu as an input TE (for seeding relation of navigation structure)
    HLT::TEVec allTEs;
    if ( (tes_in.size()>0) && (tes_in[0].size()>0) ) allTEs.push_back( tes_in[0][0] );

    // create output TE:
    // Create an output TE seeded by the inputs
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
    outputTE->setActiveState(true);

    // save (cached) feature to output TE:
    m_config->getNavigation()->copyAllFeatures( m_cachedTE, outputTE );

    return HLT::OK;

  } // end of caching

  if (timerSvc()) mTimers[ITIMER_INIT]->stop();

  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "execHLTAlgorithm called" << endreq;
 
  // start monitoring
  beforeExecMonitors().ignore();

 
  int i_cl=0;
  lvl1_muclu_roi muonClu[20];
  lvl1_muclu_roi muonClu0[20];

  HLT::TEVec allTEs;
  vector<double> VEta,VPhi;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "begin loop on TE and get muon RoIs" << endreq;
  }

  if(tes_in[0].size()==0) {
    msg() << MSG::WARNING << "Can't get any TE " << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  } else {
    for (HLT::TEVec::const_iterator it = tes_in[0].begin();it != tes_in[0].end(); ++it) {
      const TrigRoiDescriptor* roi;
      if ( HLT::OK != getFeature(*it, roi) ) {
        msg() << MSG::WARNING << "Can't get RoI for TE " << (*it)->getId()<< endreq;
        return HLT::NAV_ERROR;
      }
      if(i_cl>= kMAX_ROI) {
        msg() << MSG::WARNING << "Too many Muon RoIs: bailing out" << endreq;
        break;
      }

      VEta.push_back(roi->eta());
      VPhi.push_back(roi->phi());
        
      m_RoiEta.push_back(roi->eta());
      m_RoiPhi.push_back(roi->phi());
      lvl1_muclu_roi my_lvl1_clu_roi;
      my_lvl1_clu_roi.eta = roi->eta();
      my_lvl1_clu_roi.phi = roi->phi();
      my_lvl1_clu_roi.nroi = 0;
      muonClu[i_cl] = my_lvl1_clu_roi;
      muonClu0[i_cl] = my_lvl1_clu_roi;
      allTEs.push_back(*it);
      ++i_cl;
    }
  }

  if (msgLvl() <= MSG::DEBUG) { 
    msg() << MSG::DEBUG << "Accumulated " << i_cl << " ROI Directions: " << endreq;
    for (int unsigned i=0;i<VEta.size();i++) {
      msg() << MSG::DEBUG << "[" << VEta[i] << "," << VPhi[i] << "]" << endreq;
    }
  }
   

  int n_cl = i_cl;
 
  if (timerSvc()) mTimers[ITIMER_INIT]->stop();
  if (timerSvc()) mTimers[ITIMER_CLUSTER]->start();
  // start the clustering
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Start the Muon RoI clustering" << endreq;
  }
  for(int i_cl=0; i_cl<n_cl; ++i_cl) { // loop on cluster
    bool improvement = true;
    while(improvement){
      int n_in_clu = 0;
      for (int j_cl=0; j_cl<n_cl; ++j_cl) { // loop on cluster
	float deltaR = DeltaR(muonClu0[j_cl],muonClu[i_cl]);
	if(deltaR<mDeltaR){
	  ++n_in_clu;
	  muonClu[i_cl].eta = muonClu[i_cl].eta + (muonClu0[j_cl].eta-muonClu[i_cl].eta)/n_in_clu;
	  muonClu[i_cl].phi = HLT::wrapPhi(muonClu[i_cl].phi + HLT::wrapPhi(muonClu0[j_cl].phi-muonClu[i_cl].phi)/n_in_clu);
	}
      }
      //find the number of ROIs in the new cluster
      Int_t n_in_clu2=0;
      for (int j_cl=0; j_cl<n_cl; ++j_cl) { // loop on cluster
	float deltaR = DeltaR(muonClu0[j_cl],muonClu[i_cl]);
	if(deltaR<mDeltaR){
	  ++n_in_clu2;
	}
      }
      if (msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Finding the number of Muon RoIs in the new Cluster....   " << n_in_clu2 << endreq;
      } 
      if(n_in_clu2>muonClu[i_cl].nroi){
	muonClu[i_cl].nroi=n_in_clu2;
	improvement = true;
      } else  improvement = false;
    }
  }
  if (timerSvc()) mTimers[ITIMER_CLUSTER]->stop();
  // find the cluster with max number of rois
  int ncl_max = 0;
  for(int i_cl=0; i_cl<n_cl; ++i_cl) { // loop on cluster
    if(muonClu[i_cl].nroi>ncl_max){
      mCluEta = muonClu[i_cl].eta;
      mCluPhi = muonClu[i_cl].phi;
      mCluNum = muonClu[i_cl].nroi;
      ncl_max = muonClu[i_cl].nroi;
    }
  }
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Finding the Cluster with the maximum number of RoIs....   " << ncl_max << endreq;
  }
  

  if (timerSvc()) mTimers[ITIMER_JETS]->start();

  int njets = 0;

  const xAOD::JetContainer* vectorOfJets = 0;

  std::vector<HLT::TriggerElement*>& inputTE = tes_in.at(1);
  HLT::ErrorCode ec = getFeature(inputTE.front(), vectorOfJets);

  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get the Jets " << endreq;
    return ec;
  }

  if(vectorOfJets==0) {
    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Missing feature." << endreq;
    return HLT::MISSING_FEATURE;
  }

  if (msgLvl() <= MSG::DEBUG) { 
    msg() << MSG::DEBUG << "The number of Jets attached to the TE is " << vectorOfJets->size() << endreq;
  }

  for (xAOD::JetContainer::const_iterator jet = vectorOfJets->begin(); jet != vectorOfJets->end(); jet++) {
    
    double etjet  = (*jet)->p4().Et();
    // XAOD::JET OBJECT NEEDS TO BE DECORATED WITH EMFRAC BY UPSTREAM ALGORITHM - CAN'T DO ANYTHING AT THIS STAGE
    double ehjet  = 1;
    double emjet  = 1;
    //double ehjet  = (*jet)->getAttribute<float>(xAOD::JetAttribute::EMFrac);
    //double emjet  = (*jet)->getAttribute<float>(xAOD::JetAttribute::EMFrac);
    double jetEta = (*jet)->eta();
    double jetPhi = (*jet)->phi();
    double erat = -99999999.;
    
    double delta_eta = mCluEta - jetEta;
    double delta_phi = HLT::wrapPhi(mCluPhi - jetPhi);

    double dR = sqrt( (delta_eta*delta_eta) + (delta_phi*delta_phi) );
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "HLT jet /eta=" << jetEta << " /phi= " << jetPhi << "/Et= " << etjet << "/DR= " << dR << endreq;

    if (dR<mDeltaRJet) {
      if (emjet > 0. && ehjet >= 0.) erat = log10(ehjet/emjet);
      if (emjet <= 0. && ehjet > 0.) erat = 9999999;
      if (emjet <= 0. && ehjet <= 0.) erat = 9999999;
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Jet log10 of had / em energy ratio: " << erat << endreq;
      if (erat<=0.5) {
        njets++;
      }
    }
  }

  mNumJet = njets;

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Found " << mNumJet << " Jets with Log(H/E)<1.0" << endreq;

  if (timerSvc()) mTimers[ITIMER_JETS]->stop();

  if (timerSvc()) mTimers[ITIMER_TRACKS]->start();

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Accessing the ID track collection" << endreq;

  int ntrk = 0;
  if(m_UseID) {

    for (HLT::TEVec::const_iterator it = tes_in[0].begin();it != tes_in[0].end(); ++it) {
      std::vector<const TrigInDetTrackCollection*> vectorOfTrackCollections;
      HLT::ErrorCode ect = getFeatures(*it,vectorOfTrackCollections, "");

      if( ect!=HLT::OK ) {
        if (msgLvl() <= MSG::DEBUG) { 
          msg() << MSG::DEBUG << " Failed to get InDetTrackCollections " << endreq;
        }
      }else{
        if (msgLvl() <= MSG::DEBUG) {
          msg() << MSG::DEBUG << " Got " << vectorOfTrackCollections.size()
                << " InDetTrackCollections " << endreq;
        }
        std::vector<const TrigInDetTrackCollection*>::iterator
        theTrackColl = vectorOfTrackCollections.begin(),
        endTrackColl = vectorOfTrackCollections.end(); 
             
        for( ; theTrackColl != endTrackColl;  theTrackColl++){
           
          TrigInDetTrackCollection::const_iterator 
          track     = (*theTrackColl)->begin(),
          lasttrack = (*theTrackColl)->end();
       
          for(; track !=lasttrack; track++ ){
  
            if( (*track)->algorithmId() != m_algoId ) continue;
            if (!((*track)->param())) continue;

            double pt_idtr = (*track)->param()->pT();
            if (fabs(pt_idtr) <= m_PtMinID) continue;

            double phi_id  = (*track)->param()->phi0();
            double eta_id  = (*track)->param()->eta();

            if(msgLvl() <= MSG::DEBUG)
              msg() << MSG::DEBUG << "trying " << m_ID_algo_to_use << " track with " << "pt=" << pt_idtr << ", eta=" << eta_id << ", phi=" << phi_id  << endreq;

            double delta_etat = mCluEta - eta_id;
            double delta_phit = HLT::wrapPhi(mCluPhi - phi_id);

            double dRt = sqrt( (delta_etat*delta_etat) + (delta_phit*delta_phit) );
            if (dRt<mDeltaRTrk) {
              ntrk++;
            }
          }
        }
      }
    }
  }

  //monitored variable
  mNumTrk = ntrk;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Found " << mNumTrk << " tracks around the Muon Cluster direction" << endreq;
  }
  if (timerSvc()) mTimers[ITIMER_TRACKS]->stop();

  if (timerSvc()) mTimers[ITIMER_FINAL]->start();

  //WRITE FEATURE 
  m_clu_feature = new TrigMuonClusterFeature(mCluEta,mCluPhi, mCluNum, mNumJet, mNumTrk);

    
  // finished now debugging
  if (msgLvl() <= MSG::DEBUG) {  
    msg() << MSG::DEBUG << "Create an output TE seeded by the input" << endreq;
  }
  // Only count mu as an input TE (for seeding relation of navigation structure)
  if ( (tes_in.size()>0) && (tes_in[0].size()>0) ) allTEs.push_back( tes_in[0][0] );

  // create output TE:
  // Create an output TE seeded by the inputs
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
  outputTE->setActiveState(true);

  // save feature to output TE:
  HLT::ErrorCode hltStatus = attachFeature(outputTE, m_clu_feature, m_featureLabel);

  if ( hltStatus != HLT::OK ) {
     msg() << MSG::ERROR << "Write of TrigMuonCluster feature into outputTE failed"
           << endreq;
     return hltStatus;
  }

  if (timerSvc()) mTimers[ITIMER_FINAL]->stop();

  // CACHING
  // if we got here, everything was okay. so, we cache the feature for further execution of this instance in e.g. other MET Sequences:
  // m_useCachedResult = true;

  // CACHING / Update implemented since tag 00-00-19 / A.C.
  // Caching disabled to avoid cross-talk between similar chains
  // i.e. chains seeded by 2MU6 or 2MU10 
  m_useCachedResult = false;
  m_cachedTE = outputTE;

 
  
  //----------------------------------------------------------------
  // REGTEST
  //----------------------------------------------------------------
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " REGTEST \t Cluster with : " 
	  << mCluNum << " LVL1-Roi"  << endreq;
    msg() << MSG::DEBUG << " REGTEST \t Cluster Eta " <<  mCluEta << "  Cluster Phi " << mCluPhi << endreq;
    msg() << MSG::DEBUG << " REGTEST \t with " <<  mNumJet << " jets in the cluster cone " << endreq;
    msg() << MSG::DEBUG << " REGTEST \t and with " <<  mNumTrk << " tracks around the cluster direction " << endreq;
  }
  //----------------------------------------------------------------
  
  afterExecMonitors().ignore();

  return HLT::OK;
}


void MuonCluster::clearEvent() {
  mCluEta = 0.;
  mCluPhi = 0.;
  mCluNum = 0;
  mNumJet = -1;
  mNumTrk = -1;
}



float MuonCluster::DeltaR(std::vector<const LVL1::RecMuonRoI*>::const_iterator p_roi,lvl1_muclu_roi q_roi){

  float delPhi = HLT::wrapPhi((*p_roi)->phi()-(q_roi).phi);
  float delEta = (*p_roi)->eta()-(q_roi).eta;

  return(sqrt(delPhi*delPhi+delEta*delEta));  
}

float MuonCluster::DeltaR(lvl1_muclu_roi p_roi,lvl1_muclu_roi q_roi){

  float delPhi = HLT::wrapPhi((p_roi).phi-(q_roi).phi);
  float delEta = (p_roi).eta-(q_roi).eta;

  return(sqrt(delPhi*delPhi+delEta*delEta));
}

