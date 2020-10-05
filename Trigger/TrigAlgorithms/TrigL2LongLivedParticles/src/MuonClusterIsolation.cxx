/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
  MuonClusterIsolation.cxx
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
#include "TrigL2LongLivedParticles/MuonClusterIsolation.h"
//JETS
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"
//ID TRACKING
#include "xAODTracking/TrackParticleContainer.h"
#include "CxxUtils/fpcompare.h"
#include "CxxUtils/phihelper.h"


using namespace std;

MuonClusterIsolation::MuonClusterIsolation(const std::string& name, ISvcLocator* svc) 
  : HLT::AllTEAlgo(name, svc), m_useCachedResult(false), m_clu_feature(0), m_cachedTE(0)
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

MuonClusterIsolation::~MuonClusterIsolation() {
}

HLT::ErrorCode MuonClusterIsolation::hltInitialize() {

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

  if (doTiming()) {
    m_Timers.push_back(addTimer("ClusterMuon_Initialization"));
    m_Timers.push_back(addTimer("ClusterMuon_ClusterIsolation"));
    m_Timers.push_back(addTimer("ClusterMuon_JetIsolation"));
    m_Timers.push_back(addTimer("ClusterMuon_TrackIsolation"));
    m_Timers.push_back(addTimer("ClusterMuon_Finalization"));

    declareMonitoredObject("Initialization", *(m_Timers[ITIMER_INIT]), 
                           &TrigTimer::elapsed);
    declareMonitoredObject("JetIsolation", *(m_Timers[ITIMER_JETS]), 
                           &TrigTimer::elapsed);
    declareMonitoredObject("TrackIsolation", *(m_Timers[ITIMER_TRACKS]), 
                           &TrigTimer::elapsed);
    declareMonitoredObject("Finalization", *(m_Timers[ITIMER_FINAL]), 
                           &TrigTimer::elapsed);
  }
   
  msg() << MSG::INFO << "initialize() success" << endmsg;

  return HLT::OK;
}

HLT::ErrorCode MuonClusterIsolation::hltFinalize() {
  msg() << MSG::INFO << "hltFinalize()" << endmsg;
  return HLT::OK;
}

HLT::ErrorCode MuonClusterIsolation::hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in, unsigned int type_out)
{ 
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "hltExecute()" << endmsg;
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

  m_clu_feature = new xAOD::TrigCompositeContainer();
  xAOD::TrigCompositeAuxContainer compAux;
  m_clu_feature->setStore(&compAux);
 
  // start monitoring
  beforeExecMonitors().ignore();

  HLT::TEVec allTEs;
 
  //check to make sure we have all the input trigger elements!  
  if ( (tes_in.size() < 2) ){
	     msg() << MSG::ERROR << "input trigger element vector isn't the correct size! Aborting chain."
	           << endmsg;
	     return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  }

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "begin loop on TE and get muon RoI cluster" << endmsg;
  }
  //  xAOD::TrigComposite *compClu = new xAOD::TrigComposite();
  //m_clu_feature->push_back(compClu); //add jets to the composite container

  const xAOD::TrigCompositeContainer* vectorOfClusters = 0;
  std::vector<HLT::TriggerElement*>& inputClusterTE = tes_in.at(0);
  HLT::ErrorCode ec = getFeature(inputClusterTE.front(), vectorOfClusters);

  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get the input cluster " << endmsg;
    return ec;
  }
 
  if(vectorOfClusters->size() > 1){
    msg() << MSG::WARNING << "Too many muon RoI clusters! Decision will be made based on the last one in the container." << endmsg;
  }
 
  for (xAOD::TrigCompositeContainer::const_iterator clus = vectorOfClusters->begin(); clus != vectorOfClusters->end(); clus++) {
      if(!(*clus)->getDetail("ClusterEta", m_CluEta)) {          
          msg() << MSG::ERROR << "Failed to get input cluster eta! Aborting chain." << endmsg;
          return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE); 
      }
      if(!(*clus)->getDetail("ClusterPhi", m_CluPhi)) {      
          msg() << MSG::ERROR << "Failed to get input cluster phi! Aborting chain." << endmsg;
          return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE); 
      }
      if(!(*clus)->getDetail("nRoIs", m_CluNum)) {
	msg() << MSG::ERROR << "Failed to get input cluster nRoIs! Aborting chain." << endmsg;
        return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
      }
  }

  if (timerSvc()) m_Timers[ITIMER_INIT]->stop();
  if (timerSvc()) m_Timers[ITIMER_JETS]->start();

  int njets = 0;

  const xAOD::JetContainer* vectorOfJets = 0;

  std::vector<HLT::TriggerElement*>& inputTE = tes_in.at(1);
  ec = getFeature(inputTE.front(), vectorOfJets);

  if(ec!=HLT::OK) {
    msg() << MSG::WARNING << " Failed to get the Jets " << endmsg;
    return ec;
  }
  //Is this bad? What happens in events with zero jets? 
  if(vectorOfJets==0) {
    if (msgLvl() <= MSG::WARNING)
      msg() << MSG::WARNING << "Missing feature, no jets in this event!" << endmsg;
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
    double delta_phi = CxxUtils::wrapToPi(m_CluPhi - jetPhi);

    double dR = sqrt( (delta_eta*delta_eta) + (delta_phi*delta_phi) );
    
    if (msgLvl() <= MSG::DEBUG){
      msg() << MSG::DEBUG << "HLT jet /eta=" << jetEta << " /phi= " << jetPhi << "/Et= " << etjet << "/DR= " << dR << endmsg;
    }
    if (dR<m_DeltaRJet) {
      double zero = 0.;
      double one = 1.;
      if (CxxUtils::fpcompare::greater(jetEMF,zero)){
        if(CxxUtils::fpcompare::greater_equal(jetEMF,one)){ erat = -999.;}
        else { erat = log10(double(1./jetEMF - 1.));}
      } else {
        erat = 999; 
      }   
      if (msgLvl() <= MSG::DEBUG){ 
	   msg() << MSG::DEBUG << "Jet log10 of had/em energy ratio: " << erat << endmsg;
      }
      if (erat<=0.5) {
        njets++;
      }
    }
  } 

  m_NumJet = njets;

  if (msgLvl() <= MSG::DEBUG){
    msg() << MSG::DEBUG << "Found " << m_NumJet << " Jets with Log(H/E)<1.0 matched to the Muon RoI Cluster" << endmsg;
  }
 
  if (timerSvc()) m_Timers[ITIMER_JETS]->stop();

  if (timerSvc()) m_Timers[ITIMER_TRACKS]->start();

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Accessing the ID track collection" << endmsg;

  int ntrk = 0;
  
  if (msgLvl() <= MSG::DEBUG){
  msg() << MSG::DEBUG << "there are: " << tes_in.at(0).size() << " trigger elements with track collections." <<
		  " This should be equal to the number of MU10 RoIs" << endmsg;
  }
  
  int nTEs = 0;
  
  for (HLT::TEVec::const_iterator it = tes_in.at(0).begin();it != tes_in.at(0).end(); ++it) {
	nTEs++;
	if (msgLvl() <= MSG::DEBUG){ msg() << MSG::DEBUG << "trigger element: " << nTEs  << " of tes_in.at(0) "<< endmsg;}
		
    	std::vector<const xAOD::TrackParticleContainer*> vectorOfTrackCollections;
    	HLT::ErrorCode ect = getFeatures(*it,vectorOfTrackCollections, "InDetTrigTrackingxAODCnv_Muon_IDTrig");
    
    if( ect!=HLT::OK ) {
      if (msgLvl() <= MSG::DEBUG){
	msg() << MSG::DEBUG << " Failed to get tracks" << endmsg;
      }
    } else {
      if (msgLvl() <= MSG::DEBUG){
	msg() << MSG::DEBUG << " Got " << vectorOfTrackCollections.size() << " collections of tracks " << endmsg;
      }
      std::vector<const xAOD::TrackParticleContainer*>::iterator
	theTrackColl = vectorOfTrackCollections.begin(),
	endTrackColl = vectorOfTrackCollections.end(); 
             
      for( ; theTrackColl != endTrackColl;  theTrackColl++) {
           
	xAOD::TrackParticleContainer::const_iterator
	  track     = (*theTrackColl)->begin(),
	  lasttrack = (*theTrackColl)->end();
       
	for(; track !=lasttrack; track++ ) {
  
	  float pT = fabs(std::sin((*track)->theta())/(*track)->qOverP());

	  if ( pT <= m_PtMinID) continue;
	  double eta  = (*track)->eta();
	  double phi  = (*track)->phi0();
	  if(msgLvl() <= MSG::DEBUG){
	    msg() << MSG::DEBUG << "track with " << "pt=" << pT << ", eta=" << eta << ", phi=" << phi  << endmsg;
	  }

	  double delta_etat = m_CluEta - eta;
	  double delta_phit = CxxUtils::wrapToPi(m_CluPhi - phi);

	  double dRt = sqrt( (delta_etat*delta_etat) + (delta_phit*delta_phit) );
	  if (dRt<m_DeltaRTrk) {
	    ntrk++;
	  }
	}
      }
    }
  } //end loop over input track TEs

  //monitored variable
  m_NumTrk = ntrk;

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Found " << m_NumTrk << " tracks around the Muon Cluster direction (inaccurate, often duplicate tracks in container)" << endmsg;
  }
  
  if (timerSvc()) m_Timers[ITIMER_TRACKS]->stop();

  if (timerSvc()) m_Timers[ITIMER_FINAL]->start();

  if (msgLvl() <= MSG::DEBUG) {  
    msg() << MSG::DEBUG << "Create an output TE seeded by the input" << endmsg;
  }

  xAOD::TrigComposite *compClu = new xAOD::TrigComposite();
  m_clu_feature->push_back(compClu); //add cluster to the composite container
  compClu->setName("Cluster");
  compClu->setDetail( "nTrks", m_NumTrk);
  compClu->setDetail( "nJets", m_NumJet);
  compClu->setDetail( "ClusterEta", m_CluEta);
  compClu->setDetail( "ClusterPhi", m_CluPhi);
  compClu->setDetail( "nRoIs", m_CluNum);
 
  if (msgLvl() <= MSG::DEBUG) {
     msg() << MSG::DEBUG << "Created a new cluster that has the isolation parameters" << endmsg;
  } 
	  //add the track container
	  if(tes_in.at(0).size() > 0 ){ 
		 allTEs.push_back(tes_in.at(0).back());
      	  } else {
		 msg() << MSG::ERROR << "can't add the track container! Aborting chain." << endmsg;
                 return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
	  }
	  //add the jet container
  if( tes_in.at(1).size() > 0){ 
	allTEs.push_back(tes_in.at(1).front());
  } else {
	     msg() << MSG::ERROR << "can't add the jet container! Aborting chain." << endmsg;
	     return HLT::ErrorCode(HLT::Action::ABORT_CHAIN,  HLT::Reason::MISSING_FEATURE);
  }
  
  // create output TE:
  // Create an output TE seeded by the inputs
  HLT::TriggerElement* outputTE = config()->getNavigation()->addNode(allTEs, type_out);
  outputTE->setActiveState(true);

  // save feature to output TE:
  HLT::ErrorCode hltStatus = attachFeature(outputTE, m_clu_feature, m_featureLabel);

  if ( hltStatus != HLT::OK ) {
     msg() << MSG::ERROR << "Write of MuonRoICluster TrigCompositeContainer feature into outputTE failed" << endmsg;
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
    msg() << MSG::DEBUG << " REGTEST \t Cluster with : " << m_CluNum << " LVL1-Roi"  << endmsg;
    msg() << MSG::DEBUG << " REGTEST \t Cluster Eta " <<  m_CluEta << "  Cluster Phi " << m_CluPhi << endmsg;
    msg() << MSG::DEBUG << " REGTEST \t with " <<  m_NumJet << " jets in the cluster cone " << endmsg;
    msg() << MSG::DEBUG << " REGTEST \t and with " <<  m_NumTrk << " tracks around the cluster direction " << endmsg;
  }
  //----------------------------------------------------------------
  
  afterExecMonitors().ignore();

  return HLT::OK;
}


void MuonClusterIsolation::clearEvent() {
  m_CluEta = 0.;
  m_CluPhi = 0.;
  m_CluNum = 0;
  m_NumJet = -1;
  m_NumTrk = -1;
}

