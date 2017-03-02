/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSecVtxFinder/TrigSecVtxFinder.h"
#include "GaudiKernel/MsgStream.h"

#include "InDetBeamSpotService/IBeamCondSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include <sstream>

TrigSecVtxFinder::TrigSecVtxFinder(const std::string &name, ISvcLocator *pSvcLocator):
  HLT::FexAlgo(name, pSvcLocator),
  m_algo(6),
  m_useBeamSpotFlag(false),
  m_trackJetTrkSelExt(false),
  m_secVtx_num(0),
  m_secVtx_numTrk(0),
  m_secVtx_twoTrk(0),
  m_secVtx_mass(0),
  m_secVtx_energy(0),
  m_secVtx_dl(0),
  m_secVtx_dls(0),
  m_trackJetFinderTool("TrigTrackJetFinderTool",this),
  m_secvtxFinderTool("TrigInDetVxInJetTool",this)
{
  declareProperty ("AlgoId",             m_algo,              "Which track reconstruction algo at L2 to use");
  declareProperty ("SecVtxTool",         m_secvtxFinderTool);
  declareProperty ("UseBeamSpotFlag",    m_useBeamSpotFlag   = false);
  declareProperty ("TrackJetTrkSelExt",  m_trackJetTrkSelExt = false);
  declareProperty ("UseJetDirection",    m_useJetDirection);

  declareMonitoredVariable("SecVtx_Num",            m_secVtx_num,    AutoClear);
  declareMonitoredVariable("SecVtx_NumTrk",         m_secVtx_numTrk, AutoClear);
  declareMonitoredVariable("SecVtx_TwoTrk",         m_secVtx_twoTrk, AutoClear);
  declareMonitoredVariable("SecVtx_Mass",           m_secVtx_mass,   AutoClear);
  declareMonitoredVariable("SecVtx_Energy",         m_secVtx_energy, AutoClear);
  declareMonitoredVariable("SecVtx_DecayLength",    m_secVtx_dl,     AutoClear);
  declareMonitoredVariable("SecVtx_DecayLengthSig", m_secVtx_dls,    AutoClear);
}
  
TrigSecVtxFinder::~TrigSecVtxFinder() {
}

HLT::ErrorCode TrigSecVtxFinder::hltInitialize() {

  msg() << MSG::INFO << "TrigSecVtxFinder::initialize()" << endmsg;

  //* Retrieve TrigTrackJetFinder tool *//
  StatusCode sc = m_trackJetFinderTool.retrieve();
  if(sc.isFailure()) {
    msg() << MSG::FATAL << "Failed to locate tool " << m_trackJetFinderTool << endmsg;
    return HLT::BAD_JOB_SETUP;
  } else
    msg() << MSG::INFO << "Retrieved tool " << m_trackJetFinderTool << endmsg;

  sc = m_secvtxFinderTool.retrieve();
  if ( sc.isFailure() ){
    msg() << MSG::FATAL << "Unable to retrieve TrigInDetVxInJetTool " << m_secvtxFinderTool << endmsg;
    return HLT::BAD_JOB_SETUP;
  }
  else
    msg() << MSG::INFO << "Retrieved tool " << m_secvtxFinderTool << endmsg;

  //* declareProperty overview *//
  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "declareProperty review:" << endmsg;
    msg() << MSG::DEBUG << " AlgoId = "              << m_algo << endmsg; 
    msg() << MSG::DEBUG << " UseBeamSpotFlag = "     << m_useBeamSpotFlag << endmsg; 
    msg() << MSG::DEBUG << " TrackJetTrkSelExt = "   << m_trackJetTrkSelExt << endmsg;
    msg() << MSG::DEBUG << " UseJetDirection = "     << m_useJetDirection << endmsg; 
  }

  return HLT::OK;
}

HLT::ErrorCode TrigSecVtxFinder::hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE) {

  // ====== get RoI descriptor =========
  const TrigRoiDescriptor* roiDescriptor = 0;
  if ((getFeature(inputTE, roiDescriptor) == HLT::OK) && roiDescriptor) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Using inputTE: " 
	    << "RoI id " << roiDescriptor->roiId()
	    << ", Phi = " <<  roiDescriptor->phi()
	    << ", Eta = " << roiDescriptor->eta() << endmsg;
    }
  } else {
    if (msgLvl() <= MSG::DEBUG) 
      msg() <<  MSG::DEBUG << "No RoI for this Trigger Element" << endmsg;
    
    return HLT::NAV_ERROR;
  }

  // ====== get the necessary ingredients =========
  HLT::ErrorCode sc;
  Trk::RecVertex prmVtx;
  if ((sc = getPrmVtxForFit(prmVtx,outputTE)) != HLT::OK) {

    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "REGTEST:  RoI " << roiDescriptor->roiId() << ", Phi = "   << roiDescriptor->phi() << ", Eta = "   << roiDescriptor->eta() << endmsg;
      msg() << MSG::DEBUG << "REGTEST:  No Secondary vertex reconstructed (primary vertex not available)" << endmsg;
    }

    return sc;
  }

  if(prmVtx.fitQuality().chiSquared() == -9) {

    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "REGTEST:  RoI " << roiDescriptor->roiId() << ", Phi = "   << roiDescriptor->phi() << ", Eta = "   << roiDescriptor->eta() << endmsg;
      msg() << MSG::DEBUG << "REGTEST:  No Secondary vertex reconstructed (default primary vertex: -200 +/- -200)" << endmsg;
    }

    return HLT::OK;
  }

  if(prmVtx.fitQuality().chiSquared() == -99) {

    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "REGTEST:  RoI " << roiDescriptor->roiId() << ", Phi = "   << roiDescriptor->phi() << ", Eta = "   << roiDescriptor->eta() << endmsg;
      msg() << MSG::DEBUG << "REGTEST:  No Secondary vertex reconstructed (beam spot status not meeting requirement)" << endmsg;
    }

    return HLT::OK;
  }

  std::vector<const TrigInDetTrack*> tracks;
  if ((sc = getSelectedTracks(tracks,outputTE)) != HLT::OK) return sc;
  if (tracks.size() < 2) {

    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "REGTEST:  RoI " << roiDescriptor->roiId() << ", Phi = "   << roiDescriptor->phi() << ", Eta = "   << roiDescriptor->eta() << endmsg;
      msg() << MSG::DEBUG << "REGTEST:  No Secondary vertex reconstructed (less than 2 tracks)" << endmsg;
    }

    return HLT::OK;
  }

  CLHEP::HepLorentzVector jetDirection;
  if ((sc = getJetDirection(jetDirection, tracks, prmVtx.position().z(), inputTE)) != HLT::OK) return sc;

  // ====== turn the crank of the algorithm ==========
  const TrigVertex* secVertex =  m_secvtxFinderTool->findSecVertex(prmVtx,jetDirection,tracks);
 

  // ====== store the result ==========
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "REGTEST:  RoI " << roiDescriptor->roiId() << ", Phi = " << roiDescriptor->phi() << ", Eta = " << roiDescriptor->eta() << endmsg;

  if (!secVertex) {
    
    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST:  No sec vtx reconstructed (not found by tool)" << endmsg;
    
    return HLT::OK;

  } else {

    if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "REGTEST:  Sec vtx reconstructed: mass: " << secVertex->mass() 
	    << " dl/sigma(dl): " << secVertex->decayLengthSignificance() << endmsg;
    
    TrackInVertexList* trackInVtx = secVertex->tracks();

    m_secVtx_num++;
    m_secVtx_mass   = secVertex->mass();
    m_secVtx_twoTrk = secVertex->nTwoTracksSecVtx();
    m_secVtx_numTrk = trackInVtx->size();
    m_secVtx_energy = secVertex->energyFraction();
    m_secVtx_dl     = secVertex->decayLength();
    m_secVtx_dls    = secVertex->decayLengthSignificance();

  }

  TrigVertexCollection* secVrtContainer = new TrigVertexCollection(); 
  secVrtContainer->push_back(const_cast<TrigVertex*>(secVertex));

  if ((sc = attachFeature(outputTE, secVrtContainer, "secVrt")) != HLT::OK)
  {
    if (msgLvl() <= MSG::DEBUG) 
      msg() << MSG::DEBUG << "Cannot attach feature to TE!" << endmsg;
    return sc;
  }
  
  
  return HLT::OK;
}

HLT::ErrorCode TrigSecVtxFinder::hltFinalize() {

  msg() << MSG::INFO << "TrigSecVtxFinder::finalize()" << endmsg;
  return HLT::OK;
}

HLT::ErrorCode TrigSecVtxFinder::getPrmVtxForFit(Trk::RecVertex& vertex,
                                                 const HLT::TriggerElement* outputTE) {
  float x, y, z, exx, exy, exz, eyy, eyz, ezz;
  exy = eyz = exz = 0; // cross-correlation 0 for now

  //* Retrieve primary vertex information *//
  std::vector<const TrigVertexCollection*> vectorOfL2PrmVtxCollections;
  const TrigVertexCollection* prmVtxColl = NULL;
  if (getFeatures(outputTE, vectorOfL2PrmVtxCollections, "") != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get TrigVertexCollection from the trigger element" << endmsg;
    return HLT::NAV_ERROR;
  }

  for (int i = 0; i < (int) vectorOfL2PrmVtxCollections.size(); i++)
  {
    if (!vectorOfL2PrmVtxCollections[i] || vectorOfL2PrmVtxCollections[i]->size() == 0)
      continue;
    if (((m_algo == 1 || m_algo == 6) && vectorOfL2PrmVtxCollections[i]->front()->algorithmId() == 3) || 
        ((m_algo == 2 || m_algo == 5) && vectorOfL2PrmVtxCollections[i]->front()->algorithmId() == 10) ||
        ((m_algo == 8) && vectorOfL2PrmVtxCollections[i]->front()->algorithmId() == 19) ||
	((m_algo == 13) && vectorOfL2PrmVtxCollections[i]->front()->algorithmId() == 23) ) {
      prmVtxColl = vectorOfL2PrmVtxCollections[i];
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Retrieved prm vertex collection with algo_id " << prmVtxColl->front()->algorithmId() << endmsg;
      break;
    }
  }
  if (!prmVtxColl || prmVtxColl->size() == 0)
  {
    msg() << MSG::ERROR << "Failed to get TrigVertexCollection from the trigger element" << endmsg;
    return HLT::MISSING_FEATURE;
  }
  
  z = prmVtxColl->at(0)->z();
  ezz = prmVtxColl->at(0)->cov()[5];

  if ( z ==-200 && ezz==-200 ) {
    vertex = Trk::RecVertex(Amg::Vector3D(0.,0.,0.),0,-9);
    return HLT::OK;
  }

  //* Retrieve beamspot information *//
  IBeamCondSvc* m_iBeamCondSvc; 
  StatusCode sc = service("BeamCondSvc", m_iBeamCondSvc);

  if (sc.isFailure() || m_iBeamCondSvc == 0) {
    if (msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "Could not retrieve Beam Conditions Service. " << endmsg;
    return HLT::ERROR;
  }

  Amg::Vector3D m_beamSpot = m_iBeamCondSvc->beamPos();

  int m_beamSpotBitMap = m_iBeamCondSvc->beamStatus();

  //* Check if beam spot is from online algorithms *//
  int m_beamSpotStatus = ((m_beamSpotBitMap & 0x4) == 0x4);
  
  //* Check if beam spot fit converged *//
  if (m_beamSpotStatus)
    m_beamSpotStatus = ((m_beamSpotBitMap & 0x3) == 0x3);

  if(msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "Beam spot from service: x=" << m_beamSpot.x() << ", y=" << m_beamSpot.y() << ", z=" << m_beamSpot.z() 
	  << ", tiltXZ=" << m_iBeamCondSvc->beamTilt(0) << ", tiltYZ=" << m_iBeamCondSvc->beamTilt(1) 
	  << ", sigmaX=" << m_iBeamCondSvc->beamSigma(0) << ", sigmaY=" << m_iBeamCondSvc->beamSigma(1) << ", sigmaZ=" << m_iBeamCondSvc->beamSigma(2) 
	  << ", status=" << m_beamSpotStatus << endmsg;

  if (m_useBeamSpotFlag && !m_beamSpotStatus) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Beam spot status not meeting requirement." << endmsg;
    vertex = Trk::RecVertex(Amg::Vector3D(0.,0.,0.),0,-99);
    return HLT::OK;
  }

  x = m_beamSpot.x();
  y = m_beamSpot.y();

  //* Apply beam spot correction for tilt *//
  x = x + tan(m_iBeamCondSvc->beamTilt(0)) * (z - m_beamSpot.z());
  y = y + tan(m_iBeamCondSvc->beamTilt(1)) * (z - m_beamSpot.z());

  exx = m_iBeamCondSvc->beamSigma(0);
  eyy = m_iBeamCondSvc->beamSigma(1);

  AmgSymMatrix(3) err;  
  err(1,1) = exx;
  err(2,2) = eyy;
  err(3,3) = ezz;
  err(1,2) = err(2,1) = exy; 
  err(1,3) = err(3,1) = exz;
  err(2,3) = err(3,2) = eyz;

  AmgSymMatrix(3) cov = err;
  vertex = Trk::RecVertex(Amg::Vector3D(x,y,z), cov,
			  prmVtxColl->at(0)->ndof(), prmVtxColl->at(0)->chi2());
  return HLT::OK;
}

HLT::ErrorCode TrigSecVtxFinder::getSelectedTracks(std::vector<const TrigInDetTrack*>& tracks,
                                                   const HLT::TriggerElement* outputTE) {
  tracks.clear();
  std::vector<const TrigInDetTrackCollection*> vectorOfL2TrackCollections;
  
  if (HLT::ErrorCode status = getFeatures(outputTE, vectorOfL2TrackCollections, "") != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get InDetTrackCollection from the trigger element" << endmsg;
  } else if (msgLvl() <= MSG::DEBUG) 
    msg() << MSG::DEBUG << "Got " << vectorOfL2TrackCollections.size() << " InDetTrackCollection" << endmsg;

  std::vector<const TrigInDetTrackCollection*>::iterator trackCollection    = vectorOfL2TrackCollections.begin();
  std::vector<const TrigInDetTrackCollection*>::iterator lastTrackColl = vectorOfL2TrackCollections.end();

  for ( ; trackCollection != lastTrackColl; trackCollection++) { 
    if ((*trackCollection)->size() != 0) {
      if ((*trackCollection)->front()->algorithmId() == m_algo) {
        if (msgLvl() <= MSG::DEBUG && m_algo == 1) 
          msg() << MSG::DEBUG << "Selected collection with SiTrack label" << endmsg;
        else if (msgLvl() <= MSG::DEBUG && m_algo == 2) 
          msg() << MSG::DEBUG << "Selected collection with IdScan label" << endmsg;
	else if (msgLvl() <= MSG::DEBUG && m_algo == 5) 
	  msg() << MSG::DEBUG << "Selected collection with L2Star label (strategy A)" << endmsg;
	else if (msgLvl() <= MSG::DEBUG && m_algo == 6)
	  msg() << MSG::DEBUG << "Selected collection with L2Star label (strategy B)" << endmsg;
	else if (msgLvl() <= MSG::DEBUG && m_algo == 8)
	  msg() << MSG::DEBUG << "Selected collection with L2Star label (strategy F)" << endmsg;
        break;
      }
    }
  } 
  
  if (trackCollection == lastTrackColl)
  {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "No track collection found or colletion empty" << endmsg;  
    return HLT::OK; 
  }
  
  for (int i = 0; i < (int) (*trackCollection)->size(); i++)
    tracks.push_back((*trackCollection)->at(i));

  // TODO do track selection before calling trackJetFinder?
  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "Selected " << tracks.size() << " tracks." << endmsg;  

  return HLT::OK;
}

HLT::ErrorCode TrigSecVtxFinder::getJetDirection(CLHEP::HepLorentzVector& jetDirection,
                                                 std::vector<const TrigInDetTrack*>& tracks,
                                                 float prmVtx_z,
                                                 const HLT::TriggerElement* inputTE) {

  float etaRoI=0,    phiRoI=0;
  float etaJet=0,    phiJet=0;
  float etaTrkJet=0, phiTrkJet=0;

  //* Get the updated RoI descriptor *//
  const TrigRoiDescriptor* roiDescriptor = 0;
  if ((getFeature(inputTE, roiDescriptor) == HLT::OK) && roiDescriptor) {
    if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Using inputTE: " << "RoI id " << roiDescriptor->roiId()
	    << ", Phi = " <<  roiDescriptor->phi() << ", Eta = " << roiDescriptor->eta() << endmsg;
    }

    etaJet=roiDescriptor->eta();
    phiJet=roiDescriptor->phi();

  } else {
    if (msgLvl() <= MSG::DEBUG) 
      msg() <<  MSG::DEBUG << "No RoI for this Trigger Element" << endmsg;

    return HLT::NAV_ERROR;
  }

  //* Get the initial RoI descriptor *//
  const TrigRoiDescriptor* initialRoI = 0;
  if (getFeature(inputTE,  initialRoI, "initialRoI") == HLT::OK) {
    if (initialRoI) {
      if (msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG << "Using initialRoI: " << "RoI id " << initialRoI->roiId()
	      << ", Phi = " <<  initialRoI->phi() << ", Eta = " << initialRoI->eta() << endmsg;
      }
    } else {
      initialRoI=roiDescriptor; // L1.5 Jets...
    }

    etaRoI=initialRoI->eta();
    phiRoI=initialRoI->phi();

  } else {
    if (msgLvl() <= MSG::DEBUG) 
      msg() <<  MSG::DEBUG << "No RoI for this Trigger Element" << endmsg;
    
    return HLT::NAV_ERROR;
  }

  m_trackJetFinderTool->clear();
  m_trackJetFinderTool->inputPrimaryVertexZ(prmVtx_z);

  if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "getJetDirection() using Ext track sel = " << m_trackJetTrkSelExt << endmsg;  
  if(m_trackJetTrkSelExt) m_trackJetFinderTool->setExtTrackSel(true);

  for (unsigned int i = 0; i < (unsigned int) tracks.size(); i++) {
    const TrigInDetTrack* trk = tracks[i];
    m_trackJetFinderTool->addTrack(trk,i);
  }

  std::vector<int> tracksTrackJet;

  m_trackJetFinderTool->findJet(tracksTrackJet, etaTrkJet, phiTrkJet);

  if (etaTrkJet == -99 || phiTrkJet == -99) {
    etaTrkJet = etaRoI;
    phiTrkJet = phiRoI;
  }
  
  if (msgLvl() <= MSG::DEBUG) 
    msg() <<  MSG::DEBUG << "eta RoI " << etaRoI << " phiRoI " << phiRoI 
	  << "; eta TrkJet " << etaTrkJet << " phi TrkJet " << phiTrkJet 
	  << "; eta Jet " << etaJet << " phi Jet " << phiJet << endmsg; 

  if (m_useJetDirection == 1) {

    if (msgLvl() <= MSG::DEBUG) 
      msg() <<  MSG::DEBUG << "Using the LVL2 jet direction as input" << endmsg;

    jetDirection.setX(cos(phiJet));
    jetDirection.setY(sin(phiJet));
    jetDirection.setZ(sinh(etaJet));

  } else if (m_useJetDirection == 2) {

    if (msgLvl() <= MSG::DEBUG) 
      msg() <<  MSG::DEBUG << "Using the LVL2 track-jet direction as input" << endmsg;

    jetDirection.setX(cos(phiTrkJet));
    jetDirection.setY(sin(phiTrkJet));
    jetDirection.setZ(sinh(etaTrkJet));

  } else if (m_useJetDirection == 3) {

    if (msgLvl() <= MSG::DEBUG) 
      msg() <<  MSG::DEBUG << "Using the LVL1 RoI jet direction as input" << endmsg;

    jetDirection.setX(cos(phiRoI));
    jetDirection.setY(sin(phiRoI));
    jetDirection.setZ(sinh(etaRoI));

  }

  return HLT::OK;
}
