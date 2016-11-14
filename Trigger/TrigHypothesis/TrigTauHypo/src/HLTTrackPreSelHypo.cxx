/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     HLTTrackPreSelHypo.cxx
 * PACKAGE:  Trigger/TrigHypothesis/TrigTauHypo
 *
 * AUTHORS:   Marcelo Vogel (mavogel@cern.ch)
 *            Soshi Tsuno
 *            P.O. DeViveiros
 * CREATED:   May 26, 2014
 *
 * DESCRIPTION:
 *
 * Implementation of fast-tracking based pre-selection on
 * tau candidates starting from L1 seeds
 *
 *********************************************************************/

#include <iterator>
#include <sstream>

//
#include "TLorentzVector.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
//
#include "TrigTauHypo/HLTTrackPreSelHypo.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigSteeringEvent/PhiHelper.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "xAODTrigger/TrigPassBits.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

class ISvcLocator;

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
HLTTrackPreSelHypo::HLTTrackPreSelHypo(const std::string& name, 
				     ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator),
  m_nTracksInCore(0), 
  m_nTracksInIso(0)
  

{
  declareProperty("LowerPtCut",           m_lowerPtCut=20000.0);
  declareProperty("LowerTrackPtCut",      m_lowerTrackPtCut=0.0);
  declareProperty("ClusterEnergySumCone", m_clusterCone=0.2);
  declareProperty("TrackVariableCore",    m_coreSize=0.2);
  declareProperty("TrackVariableOuter",   m_outerSize=0.4);
  declareProperty("DeltaRLeadTrkRoI",     m_deltaRLeadTrkRoI=0.2);
  declareProperty("TracksInCoreCut",      m_tracksInCoreCut=3);
  declareProperty("TracksInIsoCut",       m_tracksInIsoCut=1);
  declareProperty("DeltaZ0Cut",           m_deltaZ0Cut=2.);
  declareProperty("rejectNoTracks",       m_rejectNoTracks=false);
  declareProperty("relaxHighPt",          m_relax_highpt=true);
  declareProperty("HighPtThreshold",      m_highpt_threshold=200000.);

  declareMonitoredVariable("CutCounter"        , m_cutCounter);
  declareMonitoredVariable("nTracksInCore"   , m_nTracksInCore);
  declareMonitoredVariable("nTracksInIso"   , m_nTracksInIso);
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
HLTTrackPreSelHypo::~HLTTrackPreSelHypo()
{  
}

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
/////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
HLT::ErrorCode HLTTrackPreSelHypo::hltInitialize()
// ----------------------------------------------------------------------
{
  
  msg() << MSG::INFO << "in initialize()" << endmsg;
  
  msg() << MSG::INFO << " REGTEST: HLTTrackPreSelHypo will cut on "                           << endmsg;
  msg() << MSG::INFO << " REGTEST: Lower pt cut for track selection: " << m_lowerTrackPtCut   << endmsg;
  msg() << MSG::INFO << " REGTEST: Tracks in core <= "                 << m_tracksInCoreCut   << endmsg;  
  msg() << MSG::INFO << " REGTEST: Tracks in outer <= "                << m_tracksInIsoCut    << endmsg;  
  msg() << MSG::INFO << " REGTEST: Relax High pT: "                    << m_relax_highpt      << endmsg;
  msg() << MSG::INFO << " REGTEST: Relax High pT Threshold: "           << m_highpt_threshold << endmsg;
  msg() << MSG::INFO << " REGTEST: ------ "                                                   << endmsg;


  msg() << MSG::INFO << "Initialization of HLTTrackPreSelHypo completed successfully" << endmsg;
  return HLT::OK;
}

/////////////////////////////////////////////////////////////////////
// FINALIZE:
/////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
HLT::ErrorCode HLTTrackPreSelHypo::hltFinalize(){
// ----------------------------------------------------------------------
  
  msg() << MSG::INFO << "in finalize()" << endmsg;
  return HLT::OK;
}


/////////////////////////////////////////////////////////////////////
// EXECUTE:
/////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------
HLT::ErrorCode HLTTrackPreSelHypo::hltExecute(const HLT::TriggerElement* inputTE, bool& pass){
// ----------------------------------------------------------------------
  
  // Get the messaging service, print where you are
  
  if( msgLvl() <= MSG::DEBUG )  
    msg() << MSG::DEBUG << "REGTEST:"<< name() << ": in execute()" << endmsg;
  
  // general reset
  m_cutCounter = 0;
  pass = false;


  //get RoI descriptor
  const TrigRoiDescriptor* roiDescriptor = 0;
  HLT::ErrorCode status = getFeature(inputTE, roiDescriptor);
  if ( status != HLT::OK || roiDescriptor == 0 ) {
    msg() <<  MSG::WARNING << " Failed to find RoiDescriptor " << endmsg;
    return status;
  }

  float roIEta = roiDescriptor->eta();
  float roIPhi = roiDescriptor->phi();
  
  //look at fast-tracks
  std::vector<const TrackCollection*> vectorFoundTracks;
  const TrackCollection* foundTracks = 0;

  //status = getFeature(inputTE, foundTracks, "TrigFastTrackFinder_Tau");

  status = getFeatures(inputTE,vectorFoundTracks);
  
  if (status !=HLT::OK) {
    msg() << MSG::ERROR << "No FastTrackFinder container was found.  Aborting pre-selection." << endmsg;
    return status;
  }
  else {

    if (vectorFoundTracks.size()<1) {
      msg() << MSG::ERROR << "FastTrackFinder vector was empty.  Aborting pre-selection." << endmsg;
      return HLT::ERROR;
    }

  }

  // Retrieve last container to be appended
  foundTracks = vectorFoundTracks.back();

  if(foundTracks) {
    msg() << MSG::DEBUG << " Input track collection has size " << foundTracks->size() << endmsg;
  }


  // retrieve the tau container
  std::vector<const xAOD::TauJetContainer*> vectorTauContainers;
  
  status = getFeatures(inputTE, vectorTauContainers);
 
  if(status!=HLT::OK) 
    {
      msg() << MSG::INFO
	    << " REGTEST: Failed to get tauContainer's from the trigger element" 
	    << endmsg;
      return HLT::OK;
    } 
  
  if( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << " Got " << vectorTauContainers.size() 
	  << " tauContainers's associated to the TE " << endmsg;
  
  if(vectorTauContainers.size() == 0)
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG << " REGTEST: Received 0 taucontainers  "
	      << "This algorithm is designed to work with  one tau container per TE."
	      << endmsg;
      return HLT::OK;
    }
  
  const xAOD::TauJetContainer *TauContainer = vectorTauContainers.back();
  msg() << MSG::DEBUG << " REGTEST: number of tau in container "<< TauContainer->size() << endmsg;



  
  if (foundTracks) {
    if (m_rejectNoTracks) {

      // ignore the 0trk cut if one taus is above m_highpt_threshold
      if (m_relax_highpt) { 
	for(xAOD::TauJetContainer::const_iterator tauIt = TauContainer->begin();
	    tauIt != TauContainer->end(); tauIt++) {
	  
	  double EFet = (*tauIt)->pt();
	  
	  if (msgLvl() <= MSG::DEBUG) {
	    msg() << MSG::DEBUG << " REGTEST: Et Calib " << EFet << endmsg;
	  }
	  // check if at least one tau passes the threshold
	  if (EFet > m_highpt_threshold) {
	    pass = true;
	    return HLT::OK;
	  }
	  
	}
      }

      // checking if the container is empty
      if (foundTracks->size() == 0) {
	msg() << MSG::DEBUG << "No Tracks in Input Collection: reject TE" << endmsg;
	pass = false;
	return HLT::OK;
      }

      // retrieving the passbits
      const xAOD::TrigPassBits* bits(0);
      HLT::ErrorCode status = getFeature(inputTE, bits, "passbits");
      if (status != HLT::OK) {
	ATH_MSG_WARNING(" Failed to get TrigPassBits ");
	return HLT::MISSING_FEATURE;
      }

      // check if at least one track satisfies the selection
      TrackCollection::const_iterator track = foundTracks->begin();
      for (track = foundTracks->begin(); track != foundTracks->end(); ++track) {
	if(bits->isPassing((*track), foundTracks)){
	  msg() << MSG::DEBUG << "track is passing selection" << endmsg;
	  pass = true;
	  return HLT::OK;
	}
      }

      // reject if no track satisfy the selection
      pass = false;
      return HLT::OK;
    }
  }

 
  if(foundTracks){
    const Trk::Track *Ltrack = 0;
    const Trk::Perigee *tp = 0;
    float trk_pt_max = 0;
    
    TrackCollection::const_iterator it = foundTracks->begin();
    TrackCollection::const_iterator itEnd = foundTracks->end();
   
    //first, identify highest pT leading track 
    for (;it!=itEnd;it++){
      const Trk::Track* track = *it;
      tp = track->perigeeParameters();
      if(tp){

	float trk_eta = tp->eta();
	float trk_phi = tp->parameters()[Trk::phi];
	double dR_trk_tau = sqrt((roIEta-trk_eta)*(roIEta-trk_eta) + HLT::wrapPhi(roIPhi-trk_phi)*HLT::wrapPhi(roIPhi-trk_phi));

	float trk_pt = tp->pT();
        if(trk_pt < m_lowerTrackPtCut) continue;
	if ((trk_pt > trk_pt_max) && dR_trk_tau < m_deltaRLeadTrkRoI) {
	  Ltrack = (*it);
	  trk_pt_max = trk_pt;
	}
      }
    }
    if(Ltrack) {
      msg() << MSG::DEBUG << " leading track pT " << trk_pt_max << endmsg;
    }
    else msg() << MSG::DEBUG << " no leading track pT, using the RoI " << endmsg;

    //next, count tracks in core and outer region with respect to leading track (or RoI)  
    float ltrk_eta;
    float ltrk_phi;
    float ltrk_z0;// for pile-up suppression cut
    bool usePileupSuppCut;

    if(Ltrack){
      ltrk_eta = Ltrack->perigeeParameters()->eta();
      ltrk_phi = Ltrack->perigeeParameters()->parameters()[Trk::phi];
      // keep using the RoI direction (updated after topoclustering) as it is done in tauRec
      //ltrk_eta = roIEta;
      //ltrk_phi = roIPhi;
      ltrk_z0 = Ltrack->perigeeParameters()->parameters()[Trk::z0];
      usePileupSuppCut = true;

    }
    else{// if no leading track use the RoI as center, but turn off pileup suppression cut
      ltrk_eta = roIEta;
      ltrk_phi = roIPhi;
      ltrk_z0 = 0.;
      usePileupSuppCut = false;
    }
    
    TrackCollection::const_iterator trackIt;
    for (trackIt=foundTracks->begin(); trackIt != foundTracks->end(); ++trackIt){
      const Trk::Track* track = *trackIt;

      tp = track->perigeeParameters();
      if(tp){
	float trk_eta = tp->eta();
	float trk_phi = tp->parameters()[Trk::phi];
	float trk_z0 = tp->parameters()[Trk::z0];
        float trk_pt = tp->pT();
        if(trk_pt < m_lowerTrackPtCut) continue;		
	float dR_trki_ltrk = sqrt((ltrk_eta-trk_eta)*(ltrk_eta-trk_eta) + HLT::wrapPhi(ltrk_phi-trk_phi)*HLT::wrapPhi(ltrk_phi-trk_phi));
	float dZ0 = fabs(ltrk_z0 - trk_z0);
	
	if((dR_trki_ltrk < m_coreSize) && ((dZ0 < m_deltaZ0Cut)||!usePileupSuppCut)){
	  ++m_nTracksInCore;
	}
	if((dR_trki_ltrk > m_coreSize) && (dR_trki_ltrk < m_outerSize) && ((dZ0 < m_deltaZ0Cut)||!usePileupSuppCut)){
	  ++m_nTracksInIso;
	}
      }
    }
  }
 
  if (m_nTracksInCore > m_tracksInCoreCut || m_nTracksInIso > m_tracksInIsoCut) 
    {
      if( msgLvl() <= MSG::DEBUG )
	msg() << MSG::DEBUG
	      << " REGTEST: Number of tracks in core(isolation) = "
	      << m_nTracksInCore
	      << " ("
	      << m_nTracksInIso
	      << ") "
              << "did not pass the number thresholds: " 
 	      << m_tracksInCoreCut
	      << " ("
	      << m_tracksInIsoCut
	      << ") "
	      << endmsg;
      return HLT::OK;
    }

  m_cutCounter++;
  pass = true;
  
  if( msgLvl() <= MSG::DEBUG )
    msg() << MSG::DEBUG << " REGTEST: TE accepted !! " << endmsg;
  
  
  return HLT::OK;
}
