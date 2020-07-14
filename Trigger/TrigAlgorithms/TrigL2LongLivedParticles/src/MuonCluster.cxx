/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
  MuonCluster.cxx
  Muon cluster finding, creates an RoI cluster for track finding
*/
#include <cmath>
#include <algorithm>
#include <sstream>
#include "GaudiKernel/ITHistSvc.h"
#include "PathResolver/PathResolver.h"
#include "TrigInterfaces/FexAlgo.h"
//LVL1 ROIS
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//TIMERS
#include "TrigTimeAlgs/TrigTimer.h"
//MUON CLUSTER
#include "TrigL2LongLivedParticles/MuonCluster.h"
#include "CxxUtils/fpcompare.h"
#include "CxxUtils/phihelper.h"

using namespace std;

MuonCluster::MuonCluster(const std::string& name, ISvcLocator* svc) 
  : HLT::AllTEAlgo(name, svc), m_useCachedResult(false), m_clu_feature(0), m_cachedTE(0)
{

  m_CluEta = 0.;
  m_CluPhi = 0.;
  m_CluNum = 0;

  declareProperty("DeltaR",       m_DeltaR=0.4,                    "radius of the muon cluster");
  declareProperty("MuonCluLabel", m_featureLabel = "MuonClusterInput", "label for the MuonCluster feature in the HLT Navigation, for the xAOD::TrigCompositeContainer");

  declareMonitoredVariable("RoIEta", m_CluEta);
  declareMonitoredVariable("RoIPhi", m_CluPhi);
  declareMonitoredVariable("NumClu", m_CluNum);
  declareMonitoredStdContainer("RoiEta",m_RoiEta);
  declareMonitoredStdContainer("RoiPhi",m_RoiPhi);

}

MuonCluster::~MuonCluster() {
}

HLT::ErrorCode MuonCluster::hltInitialize() {

  msg() << MSG::INFO << "initialize()" << endmsg;

  msg() << MSG::INFO << "Parameters for MuonCluster:" << name() << endmsg;
  msg() << MSG::INFO << "DeltaR : " << m_DeltaR << endmsg;
  msg() << MSG::INFO << "MuonCluLabel : " << m_featureLabel << endmsg;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Retrieve service StoreGateSvc" << endmsg;
  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Timers are initializated here" << endmsg;
  }


  if (doTiming()) {
    m_Timers.push_back(addTimer("ClusterMuon_Initialization"));
    m_Timers.push_back(addTimer("ClusterMuon_ClusterFinding"));
    m_Timers.push_back(addTimer("ClusterMuon_Finalization"));

    declareMonitoredObject("Initialization", 
                           *(m_Timers[ITIMER_INIT]), 
                           &TrigTimer::elapsed);
    declareMonitoredObject("ClusterFinder", *(m_Timers[ITIMER_CLUSTER]), 
                           &TrigTimer::elapsed);
    declareMonitoredObject("Finalization", *(m_Timers[ITIMER_FINAL]), 
                           &TrigTimer::elapsed);
  }
   
  msg() << MSG::INFO << "initialize() success" << endmsg;

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
  // nothing is seeded by 2MU6 though. does this code even work right?

  if (m_useCachedResult) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Executing this " << name() << " in cached mode" << endmsg;
    }

    // Only count mu as an input TE (for seeding relation of navigation structure)
    HLT::TEVec allTEs;
    if ( (tes_in.size()>0) && (tes_in[0].size()>0) ){
    	allTEs.push_back( tes_in[0][0] );//this is a list of L1 Muon RoIs
    }
    if( tes_in.size() > 1) allTEs.push_back(tes_in.at(1).front());//this should be impossible now (jets in MuonClusterIsolation)

    // create output TE:
    // Create an output TE seeded by the inputs
    HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
    outputTE->setActiveState(true);

    // save (cached) feature to output TE:
    m_config->getNavigation()->copyAllFeatures( m_cachedTE, outputTE );

    return HLT::OK;

  } // end of caching

  if (timerSvc()) m_Timers[ITIMER_INIT]->stop();

  if (msgLvl() <= MSG::DEBUG){ 
    msg() << MSG::DEBUG << "execHLTAlgorithm called" << endmsg;
  }
  //Setup the composite container we will put the MuonRoICluster in
  m_clu_feature = new xAOD::TrigCompositeContainer();
  xAOD::TrigCompositeAuxContainer compAux;
  m_clu_feature->setStore(&compAux);
  
  // start monitoring
  beforeExecMonitors().ignore();

  //check to make sure we have all the input trigger elements!  
  if ( (tes_in.size() < 1) ){ //should be the L1 Muon RoI container
	     msg() << MSG::ERROR << "input trigger element vector isn't the correct size! Aborting chain."
	           << endmsg;
	     return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  }
  
  int iter_cl=0;
  lvl1_muclu_roi muonClu[20];
  lvl1_muclu_roi muonClu0[20];

  HLT::TEVec allTEs;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "begin loop on TE and get muon RoIs" << endmsg;
  }

  if(tes_in.at(0).size()==0) {
    msg() << MSG::WARNING << "Can't get any TEs " << endmsg;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  } else {
    for (HLT::TEVec::const_iterator it = tes_in.at(0).begin();it != tes_in.at(0).end(); ++it) {
      const TrigRoiDescriptor* roi;
      if ( HLT::OK != getFeature(*it, roi) ) {
        msg() << MSG::WARNING << "Can't get RoI for TE " << (*it)->getId()<< endmsg;
        return HLT::NAV_ERROR;
      }
      if(iter_cl>= kMAX_ROI) {
        msg() << MSG::WARNING << "Too many Muon RoIs: bailing out" << endmsg;
        break;
      }

      m_RoiEta.push_back(roi->eta());
      m_RoiPhi.push_back(roi->phi());
      lvl1_muclu_roi my_lvl1_clu_roi;
      my_lvl1_clu_roi.eta = roi->eta();
      my_lvl1_clu_roi.phi = roi->phi();
      my_lvl1_clu_roi.nroi = 0;
      muonClu[iter_cl] = my_lvl1_clu_roi;
      muonClu0[iter_cl] = my_lvl1_clu_roi;
      allTEs.push_back(*it);
      ++iter_cl;
    }
  }
  int n_cl = iter_cl;
 
  if (msgLvl() <= MSG::DEBUG) { 
    msg() << MSG::DEBUG << "Accumulated " << n_cl << " ROI Directions: " << endmsg;
    for (int unsigned i=0;i<m_RoiEta.size();i++) {
      msg() << MSG::DEBUG << "[" << m_RoiEta.at(i) << "," << m_RoiPhi.at(i) << "]" << endmsg;
    }
  }
 
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
                  muonClu[i_cl].phi = CxxUtils::wrapToPi(muonClu[i_cl].phi + CxxUtils::wrapToPi(muonClu0[j_cl].phi-muonClu[i_cl].phi)/n_in_clu);
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
  if (timerSvc()){ m_Timers[ITIMER_CLUSTER]->stop();}
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
    msg() << MSG::DEBUG << "Found the Cluster with the maximum number of RoIs....   " << ncl_max << endmsg;
  }
  if (timerSvc()){ m_Timers[ITIMER_FINAL]->start();}

  // finished now debugging
  if (msgLvl() <= MSG::DEBUG) {  
    msg() << MSG::DEBUG << "Create an output TE seeded by the input" << endmsg;
  }
  //create an outputTE
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
  outputTE->setActiveState(true);
  
  xAOD::TrigComposite *compClu = new xAOD::TrigComposite();
  m_clu_feature->push_back(compClu); //add jets to the composite container
  compClu->setName("Cluster");
  compClu->setDetail( "ClusterEta", m_CluEta);
  compClu->setDetail( "ClusterPhi", m_CluPhi);
  compClu->setDetail( "nRoIs", m_CluNum);
  
  //create a TrigRoiDescriptor to send to ID tracking, to seed track-finding
  //only need to do this if the MuonCluster will pass the hypo!
  if( (m_CluNum >= 3 && fabs(m_CluEta) < 1.0) || (m_CluNum >= 4 && fabs(m_CluEta) >= 1.0 && fabs(m_CluEta) <= 2.5)){
    double phiHalfWidth = 0.35;
    double phiMinus = CxxUtils::wrapToPi(m_CluPhi-phiHalfWidth);
    double phiPlus  = CxxUtils::wrapToPi(m_CluPhi+phiHalfWidth);
    TrigRoiDescriptor* roi =  new TrigRoiDescriptor(m_CluEta, m_CluEta-0.4, m_CluEta+0.4,m_CluPhi, phiMinus, phiPlus);
    
    HLT::ErrorCode hltStatus = attachFeature(outputTE, roi, "forID");
    if ( hltStatus != HLT::OK ) {
      msg() << MSG::ERROR << "Failed to attach TrigRoiDescriptor as feature " << *roi << endmsg;
      return hltStatus;
    }
  }
  
  // save trigger cluster feature to output TE:
  HLT::ErrorCode hltStatus = attachFeature(outputTE, m_clu_feature, m_featureLabel);

  if ( hltStatus != HLT::OK ) {
     msg() << MSG::ERROR << "Write of MuonRoICluster TrigCompositeContainer feature into outputTE failed"
           << endmsg;
     return hltStatus;
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
  }
  //----------------------------------------------------------------
  
  afterExecMonitors().ignore();

  return HLT::OK;
}

void MuonCluster::clearEvent() {
  m_CluEta = 0.;
  m_CluPhi = 0.;
  m_CluNum = 0;
}

float MuonCluster::DeltaR(std::vector<const LVL1::RecMuonRoI*>::const_iterator p_roi,lvl1_muclu_roi q_roi){

  float delPhi = CxxUtils::wrapToPi((*p_roi)->phi()-(q_roi).phi);
  float delEta = (*p_roi)->eta()-(q_roi).eta;

  return(sqrt(delPhi*delPhi+delEta*delEta));  
}

float MuonCluster::DeltaR(lvl1_muclu_roi p_roi,lvl1_muclu_roi q_roi){

  float delPhi = CxxUtils::wrapToPi((p_roi).phi-(q_roi).phi);
  float delEta = (p_roi).eta-(q_roi).eta;

  return(sqrt(delPhi*delPhi+delEta*delEta));
}
