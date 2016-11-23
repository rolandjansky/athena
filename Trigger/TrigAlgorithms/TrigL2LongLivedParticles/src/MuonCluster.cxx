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
#include "xAODTracking/TrackParticleContainer.h"
#include "CxxUtils/fpcompare.h"

using namespace std;

MuonCluster::MuonCluster(const std::string& name, ISvcLocator* svc) 
  : HLT::AllTEAlgo(name, svc), m_useCachedResult(false),m_old_feature(0), m_clu_feature(0), m_cachedTE(0)
{

  m_CluEta = 0.;
  m_CluPhi = 0.;
  m_CluNum = 0;
  m_NumJet = -1;
  m_NumTrk = -1;

  declareProperty("DeltaR",       m_DeltaR=0.4,                    "radius of the muon cluster");
  declareProperty("DeltaRJet",    m_DeltaRJet=0.7,                 "distance between jet and muon cluster");
  declareProperty("DeltaRTrk",    m_DeltaRTrk=0.4,                 "distance between track and muon cluster");
  declareProperty("MuonCluLabel", m_featureLabel = "MuonRoICluster", "label for the MuonCluster feature in the HLT Navigation, for the xAOD::TrigCompositeContainer");
  declareProperty("MuonCluLabel_old", m_featureLabelOld = "MuonCluster", "label for the MuonCluster feature in the HLT Navigation, for the TrigMuonClusterFeature container");
  declareProperty("MinJetEt",     m_minJetEt = 30000.0,             "minimum Et (MeV) to consider a jet for isolation");
  declareProperty("PtMinID",      m_PtMinID = 5000.0,             "minimum Pt (MeV) for ID track");

  declareMonitoredVariable("RoIEta", m_CluEta);
  declareMonitoredVariable("RoIPhi", m_CluPhi);
  declareMonitoredVariable("NumClu", m_CluNum);
  declareMonitoredVariable("NumJet", m_NumJet);
  declareMonitoredVariable("NumTrk", m_NumTrk);
  declareMonitoredStdContainer("RoiEta",m_RoiEta);
  declareMonitoredStdContainer("RoiPhi",m_RoiPhi);

}

MuonCluster::~MuonCluster() {
}

HLT::ErrorCode MuonCluster::hltInitialize() {

  msg() << MSG::INFO << "initialize()" << endmsg;

  msg() << MSG::INFO << "Parameters for MuonCluster:" << name() << endmsg;
  msg() << MSG::INFO << "DeltaR : " << m_DeltaR << endmsg;
  msg() << MSG::INFO << "DeltaRJet : " << m_DeltaRJet << endmsg;
  msg() << MSG::INFO << "DeltaRTrk : " << m_DeltaRTrk << endmsg;
  msg() << MSG::INFO << "MuonCluLabel : " << m_featureLabel << endmsg;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Retrieve service StoreGateSvc" << endmsg;
  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Timers are initializated here" << endmsg;
  }

  m_Timers.push_back(addTimer("ClusterMuon_Initialization"));
  m_Timers.push_back(addTimer("ClusterMuon_ClusterFinding"));
  m_Timers.push_back(addTimer("ClusterMuon_JetsFinding"));
  m_Timers.push_back(addTimer("ClusterMuon_tracksFinding"));
  m_Timers.push_back(addTimer("ClusterMuon_Finalization"));

  declareMonitoredObject("Initialization", 
			 *(m_Timers[ITIMER_INIT]), 
			 &TrigTimer::elapsed);
  declareMonitoredObject("ClusterFinder", *(m_Timers[ITIMER_CLUSTER]), 
			 &TrigTimer::elapsed);
  declareMonitoredObject("JetsFinder", *(m_Timers[ITIMER_JETS]), 
			 &TrigTimer::elapsed);
  declareMonitoredObject("tracksFinder", *(m_Timers[ITIMER_TRACKS]), 
			 &TrigTimer::elapsed);
  declareMonitoredObject("Finalization", *(m_Timers[ITIMER_FINAL]), 
			 &TrigTimer::elapsed);
   
  msg() << MSG::INFO << "initialize() success" << endmsg;

  return HLT::OK;
}

HLT::ErrorCode MuonCluster::hltBeginRun() {
  msg() << MSG::INFO << "hltBeginRun"<<  endmsg;
  return HLT::OK;
}

HLT::ErrorCode MuonCluster::hltEndRun() {
  msg() << MSG::INFO << "hltEndRun"<<  endmsg;
  return HLT::OK;
}

HLT::ErrorCode MuonCluster::hltFinalize() {
  msg() << MSG::INFO << "hltFinalize()" << endmsg;
  return HLT::OK;
}

HLT::ErrorCode MuonCluster::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out)
{ 
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "execute()" << endmsg;
  }
  
  clearEvent();

  // CACHING
  // first check whether we executed this instance before:
  // CACHING / Update implemented since tag 00-00-19 / A.C.
  // Caching disabled to avoid cross-talk between similar chains
  // i.e. chains seeded by 2MU6 or 2MU10 
  if (m_useCachedResult) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Executing this " << name() << " in cached mode" << endmsg;
    }

    // Only count mu as an input TE (for seeding relation of navigation structure)
    HLT::TEVec allTEs;
    if ( (tes_in.size()>0) && (tes_in[0].size()>0) ){
    	allTEs.push_back( tes_in[0][0] );
    }
    if( tes_in.size() > 1) allTEs.push_back(tes_in.at(1).front());

    // create output TE:
    // Create an output TE seeded by the inputs
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
    outputTE->setActiveState(true);

    // save (cached) feature to output TE:
    m_config->getNavigation()->copyAllFeatures( m_cachedTE, outputTE );

    return HLT::OK;

  } // end of caching

  if (timerSvc()) m_Timers[ITIMER_INIT]->stop();

  if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "execHLTAlgorithm called" << endmsg;
 
  //Setup the composite container we will put the MuonRoICluster in
  m_clu_feature = new xAOD::TrigCompositeContainer();
  xAOD::TrigCompositeAuxContainer compAux;
  m_clu_feature->setStore(&compAux);
  
  // start monitoring
  beforeExecMonitors().ignore();

  //check to make sure we have all the input trigger elements!  
  if ( (tes_in.size() < 2) ){
	     msg() << MSG::ERROR << "input trigger element vector isn't the correct size! Aborting chain."
	           << endmsg;
	     return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  }
  
  int i_cl=0;
  lvl1_muclu_roi muonClu[20];
  lvl1_muclu_roi muonClu0[20];

  HLT::TEVec allTEs;
  vector<double> VEta,VPhi;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "begin loop on TE and get muon RoIs" << endmsg;
  }

  if(tes_in[0].size()==0) {
    msg() << MSG::WARNING << "Can't get any TE " << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  } else {
    for (HLT::TEVec::const_iterator it = tes_in[0].begin();it != tes_in[0].end(); ++it) {
      const TrigRoiDescriptor* roi;
      if ( HLT::OK != getFeature(*it, roi) ) {
        msg() << MSG::WARNING << "Can't get RoI for TE " << (*it)->getId()<< endmsg;
        return HLT::NAV_ERROR;
      }
      if(i_cl>= kMAX_ROI) {
        msg() << MSG::WARNING << "Too many Muon RoIs: bailing out" << endmsg;
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
    msg() << MSG::DEBUG << "Accumulated " << i_cl << " ROI Directions: " << endmsg;
    for (int unsigned i=0;i<VEta.size();i++) {
      msg() << MSG::DEBUG << "[" << VEta[i] << "," << VPhi[i] << "]" << endmsg;
    }
  }
   
  int n_cl = i_cl;
 
  if (timerSvc()) m_Timers[ITIMER_INIT]->stop();
  if (timerSvc()) m_Timers[ITIMER_CLUSTER]->start();
  // start the clustering
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Start the Muon RoI clustering" << endmsg;
  }
  for(int i_cl=0; i_cl<n_cl; ++i_cl) { // loop on cluster
    bool improvement = true;
    int n_itr = 0;
    while(improvement){
      ++n_itr;
      double eta_avg=0.0;
      double cosPhi_avg=0.0;
      double sinPhi_avg=0.0;
      int n_in_clu = 0;
      for (int j_cl=0; j_cl<n_cl; ++j_cl) { // loop on cluster
          float deltaR = DeltaR(muonClu0[j_cl],muonClu[i_cl]);
          if(deltaR<m_DeltaR){
              ++n_in_clu;
              if(n_itr==1){
                  muonClu[i_cl].eta = muonClu[i_cl].eta + (muonClu0[j_cl].eta-muonClu[i_cl].eta)/n_in_clu;
                  muonClu[i_cl].phi = HLT::wrapPhi(muonClu[i_cl].phi + HLT::wrapPhi(muonClu0[j_cl].phi-muonClu[i_cl].phi)/n_in_clu);
              } else{
                  //to recalculate the average with all RoIs within a dR = 0.4 cone of the seed position
                  eta_avg += muonClu0[j_cl].eta;
                  cosPhi_avg += cos(muonClu0[j_cl].phi);
                  sinPhi_avg += sin(muonClu0[j_cl].phi);
              }
          }
      }
      if(n_itr > 1){
      //set cluster position as average position of RoIs
      //This, coupled with the improvement=true/false below, makes an assumption that
      //improvement = false means same # RoIs in cluster, but never less (code had this before, too)
        muonClu[i_cl].eta = eta_avg/n_in_clu;  
        muonClu[i_cl].phi = atan2(sinPhi_avg,cosPhi_avg);      
      }
      //find the number of ROIs in the new cluster
      //if the number is the same as before, 
      Int_t n_in_clu2=0;
      for (int j_cl=0; j_cl<n_cl; ++j_cl) { // loop on cluster
          float deltaR = DeltaR(muonClu0[j_cl],muonClu[i_cl]);
          if(deltaR<m_DeltaR){
              ++n_in_clu2;
          }
      }
      if (msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << "Finding the number of Muon RoIs in the new Cluster....   " << n_in_clu2 << endmsg;
      } 
      if(n_in_clu2>muonClu[i_cl].nroi){
          muonClu[i_cl].nroi=n_in_clu2;
        improvement = true;
      } else  improvement = false;
    }//end while
  }
  if (timerSvc()) m_Timers[ITIMER_CLUSTER]->stop();
  // find the cluster with max number of rois
  int ncl_max = 0;
  for(int i_cl=0; i_cl<n_cl; ++i_cl) { // loop on cluster
    if(muonClu[i_cl].nroi>ncl_max){
      m_CluEta = muonClu[i_cl].eta;
      m_CluPhi = muonClu[i_cl].phi;
      m_CluNum = muonClu[i_cl].nroi;
      ncl_max = muonClu[i_cl].nroi;
    }
  }
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Finding the Cluster with the maximum number of RoIs....   " << ncl_max << endmsg;
  }
  

  if (timerSvc()) m_Timers[ITIMER_JETS]->start();

  int njets = 0;

  const xAOD::JetContainer* vectorOfJets = 0;

  std::vector<HLT::TriggerElement*>& inputTE = tes_in.at(1);
  HLT::ErrorCode ec = getFeature(inputTE.front(), vectorOfJets);

  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get the Jets " << endmsg;
    return ec;
  }

  if(vectorOfJets==0) {
    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Missing feature." << endmsg;
    return HLT::MISSING_FEATURE;
  }

  if (msgLvl() <= MSG::DEBUG) { 
    msg() << MSG::DEBUG << "The number of Jets attached to the TE is " << vectorOfJets->size() << endmsg;
  }

  for (xAOD::JetContainer::const_iterator jet = vectorOfJets->begin(); jet != vectorOfJets->end(); jet++) {
    
    double etjet  = (*jet)->p4().Et();
    if(etjet <= m_minJetEt) continue;

    double jetEMF = (*jet)->getAttribute<float>("EMFrac");
    double jetEta = (*jet)->eta();
    double jetPhi = (*jet)->phi();
    double erat = -99999999.;
    
    double delta_eta = m_CluEta - jetEta;
    double delta_phi = HLT::wrapPhi(m_CluPhi - jetPhi);

    double dR = sqrt( (delta_eta*delta_eta) + (delta_phi*delta_phi) );
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "HLT jet /eta=" << jetEta << " /phi= " << jetPhi << "/Et= " << etjet << "/DR= " << dR << endmsg;

    if (dR<m_DeltaRJet) {
      double zero = 0.;
      double one = 1.;
      if (CxxUtils::fpcompare::greater(jetEMF,zero)){
        if(CxxUtils::fpcompare::greater_equal(jetEMF,one)) erat = -999.;
        else erat = log10(double(1./jetEMF - 1.));
      } else {
        erat = 999; 
      }   
      if (msgLvl() <= MSG::DEBUG) 
	    msg() << MSG::DEBUG << "Jet log10 of had/em energy ratio: " << erat << endmsg;
      
      if (erat<=0.5) {
        njets++;
      }
    }
  } 

  m_NumJet = njets;

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Found " << m_NumJet << " Jets with Log(H/E)<1.0" << endmsg;

  if (timerSvc()) m_Timers[ITIMER_JETS]->stop();

  if (timerSvc()) m_Timers[ITIMER_TRACKS]->start();

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Accessing the ID track collection" << endmsg;

  int ntrk = 0;
  
  if (msgLvl() <= MSG::DEBUG){
  msg() << MSG::DEBUG << "there are: " << tes_in[0].size() << " trigger elements with track collections." <<
		  " This should be equal to the number of MU10 RoIs" << std::endl;
  }
  
  int nTEs = 0;
  
  for (HLT::TEVec::const_iterator it = tes_in[0].begin();it != tes_in[0].end(); ++it) {
	
	nTEs++;
	if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "trigger element: " << nTEs  << " of tes_in[0] "<< endmsg;
		
    std::vector<const xAOD::TrackParticleContainer*> vectorOfTrackCollections;
    HLT::ErrorCode ect = getFeatures(*it,vectorOfTrackCollections, "InDetTrigTrackingxAODCnv_Muon_IDTrig");
    
    if( ect!=HLT::OK ) {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Failed to get tracks" << endmsg;
    } else {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << " Got " << vectorOfTrackCollections.size() << " collections of tracks " << endmsg;

      std::vector<const xAOD::TrackParticleContainer*>::iterator
	theTrackColl = vectorOfTrackCollections.begin(),
	endTrackColl = vectorOfTrackCollections.end(); 
             
      for( ; theTrackColl != endTrackColl;  theTrackColl++) {
           
	xAOD::TrackParticleContainer::const_iterator
	  track     = (*theTrackColl)->begin(),
	  lasttrack = (*theTrackColl)->end();
       
	for(; track !=lasttrack; track++ ) {
  
	  float theta   = (*track)->theta();
	  float qOverPt = (*track)->qOverP()/TMath::Sin(theta);
	  float pT      = (1/qOverPt);

	  if (fabs(pT) <= m_PtMinID) continue;
	  
	  double phi  = (*track)->phi0();
	  double eta  = (*track)->eta();

	  if(msgLvl() <= MSG::DEBUG)
	    msg() << MSG::DEBUG << "track with " << "pt=" << pT << ", eta=" << eta << ", phi=" << phi  << endmsg;

	  double delta_etat = m_CluEta - eta;
	  double delta_phit = HLT::wrapPhi(m_CluPhi - phi);

	  double dRt = sqrt( (delta_etat*delta_etat) + (delta_phit*delta_phit) );
	  if (dRt<m_DeltaRTrk) {
	    ntrk++;
	  }
	}
      }
    }
  }

  //monitored variable
  m_NumTrk = ntrk;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Found " << m_NumTrk << " tracks around the Muon Cluster direction (inaccurate, often duplicate tracks in container)" << endmsg;
  }
  
  if (timerSvc()) m_Timers[ITIMER_TRACKS]->stop();

  if (timerSvc()) m_Timers[ITIMER_FINAL]->start();

  //WRITE FEATURE 
  m_old_feature = new TrigMuonClusterFeature(m_CluEta,m_CluPhi, m_CluNum, m_NumJet, m_NumTrk);
    
  // finished now debugging
  if (msgLvl() <= MSG::DEBUG) {  
    msg() << MSG::DEBUG << "Create an output TE seeded by the input" << endmsg;
  }
  xAOD::TrigComposite *compClu = new xAOD::TrigComposite();
  m_clu_feature->push_back(compClu); //add jets to the composite container
  compClu->setName("Cluster");
  compClu->setDetail( "nTrks", m_NumTrk);
  compClu->setDetail( "nJets", m_NumJet);
  compClu->setDetail( "ClusterEta", m_CluEta);
  compClu->setDetail( "ClusterPhi", m_CluPhi);
  compClu->setDetail( "nRoIs", m_CluNum);
  
  
  if ( (tes_in.size()>1) && (tes_in.at(0).size()>0) ){
	  //add each muon RoI and its associated features (tracks)
	  for(unsigned int iTE=0; iTE < tes_in.at(0).size(); iTE++){ allTEs.push_back( tes_in.at(0).at(iTE) ); }
	  //add the jet container
	  if( tes_in.at(1).size() > 0){ allTEs.push_back(tes_in.at(1).front());}
  } else {
	     msg() << MSG::ERROR << "input trigger element vector isn't the correct size! Aborting chain."
	           << endmsg;
	     return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  }
  
  // create output TE:
  // Create an output TE seeded by the inputs
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
  outputTE->setActiveState(true);

  // save feature to output TE:
  HLT::ErrorCode hltStatus = attachFeature(outputTE, m_clu_feature, m_featureLabel);
  HLT::ErrorCode hltStatusOld = attachFeature(outputTE, m_old_feature, m_featureLabelOld);

  if ( hltStatus != HLT::OK ) {
     msg() << MSG::ERROR << "Write of MuonRoICluster TrigCompositeContainer feature into outputTE failed"
           << endmsg;
     return hltStatus;
  }
  if (hltStatusOld != HLT::OK ) {
     msg() << MSG::ERROR << "Write of MuonCluster TrigMuonCluster feature into outputTE failed"
           << endmsg;
     return hltStatusOld;
  }  

  if (timerSvc()) m_Timers[ITIMER_FINAL]->stop();

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
	  << m_CluNum << " LVL1-Roi"  << endmsg;
    msg() << MSG::DEBUG << " REGTEST \t Cluster Eta " <<  m_CluEta << "  Cluster Phi " << m_CluPhi << endmsg;
    msg() << MSG::DEBUG << " REGTEST \t with " <<  m_NumJet << " jets in the cluster cone " << endmsg;
    msg() << MSG::DEBUG << " REGTEST \t and with " <<  m_NumTrk << " tracks around the cluster direction " << endmsg;
  }
  //----------------------------------------------------------------
  
  afterExecMonitors().ignore();

  return HLT::OK;
}


void MuonCluster::clearEvent() {
  m_CluEta = 0.;
  m_CluPhi = 0.;
  m_CluNum = 0;
  m_NumJet = -1;
  m_NumTrk = -1;
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

