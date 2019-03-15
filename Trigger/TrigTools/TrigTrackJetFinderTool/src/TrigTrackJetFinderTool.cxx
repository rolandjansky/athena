/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *********************************************************
//
// NAME.cxx
// PACKAGE:  Trigger/TrigTools/TrigTrackJetFinderTool
//
// AUTHOR:   Andrea Coccaro    (Andrea.Coccaro@ge.infn.it)
//           Andrew Lowe       (Andrew.Lowe@cern.ch)
//           Ariel Schwartzman (sch@slac.stanford.edu)
// 
// *********************************************************

#include "TrigTrackJetFinderTool/TrigTrackJetFinderTool.h"
#include "TMath.h"

using namespace std;
using std::vector;



//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrackAttributesSortPredicate(const TrackAttributes& lhs, const TrackAttributes& rhs) {

  return fabs(lhs.Pt()) > fabs(rhs.Pt());
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigTrackJetFinderTool::TrigTrackJetFinderTool(const std::string& t, const std::string& n, const IInterface*  p) :
  AthAlgTool(t,n,p),
  m_pi(3.14159265358979323846)
{
  declareInterface<ITrigTrackJetFinderTool>(this);

  declareProperty("DeltaR", m_dr     = 0.4);
  declareProperty("DeltaZ", m_dz     = 10.0);
  declareProperty("PtMin",  m_ptMin  = 0.5);
  declareProperty("PtJet",  m_ptJet  = 0.0);
  declareProperty("PtSeed", m_ptSeed = 2.0);
  declareProperty("Mult",   m_mult   = 1);

  declareProperty("DefTrkSel", defTrackSel = true);
  declareProperty("ExtTrkSel", extTrackSel = false);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigTrackJetFinderTool::~TrigTrackJetFinderTool() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode TrigTrackJetFinderTool::initialize() {
  
  StatusCode sc = AlgTool::initialize();

  msg(MSG::INFO) << "Initializing TrigTrackJetFinderTool" << endreq;

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "declareProperty review:" << endreq;
    msg(MSG::DEBUG) << " cone size (R,Z)      = " << m_dr << ", " << m_dz << endreq; 
    msg(MSG::DEBUG) << " minimum seed pT      = " << m_ptSeed << endreq;
    msg(MSG::DEBUG) << " minimum jet pT       = " << m_ptJet  << endreq;
    msg(MSG::DEBUG) << " minimum track pT     = " << m_ptMin  << endreq;
    msg(MSG::DEBUG) << " minimum multiplicity = " << m_mult  << endreq;
  }

  return sc;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigTrackJetFinderTool::clear() {

  if (msgLvl(MSG::VERBOSE))
    msg(MSG::VERBOSE) << "Clearing internal track classe" << endreq;

  m_attributesOfTracks.clear();
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigTrackJetFinderTool::addTrack(const TrigInDetTrack*& track, unsigned int i) {

  bool selTrack=true;

  if (defTrackSel) {

    if (fabs(track->param()->pT()) < m_ptMin && (fabs(track->param()->z0() - m_zpv) >= m_dz)) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track " << i+1 << " not selected (pT && deltaZ cut)" << endreq;
      selTrack=false;
    }

  }

  if (extTrackSel) {

    m_trkSelPt     = 1000;
    m_trkSelSiHits = 4;
    m_trkSelBLayer = 1;
    m_trkSelChi2   = 0.001;

    if (fabs(track->param()->pT()) < m_trkSelPt) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track " << i+1 << " not selected (pT cut)" << endreq;
      selTrack=false;
    }
    if (track->siSpacePoints()) {
      
      if ((*track->siSpacePoints())[0]->layer()) {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track " << i+1 << " not selected (missing b-layer hit)" << endreq;
        selTrack=false;
      }
      if ((int)track->siSpacePoints()->size() < m_trkSelSiHits) {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track " << i+1 << " not selected (too few silicon hits)" << endreq;
        selTrack=false;
      }
      if (TMath::Prob(track->chi2(),(int)track->siSpacePoints()->size()*3-5) <= m_trkSelChi2) {
      	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track " << i+1 << " not selected (chi2 cut)" << endreq;
        selTrack=false;
      }
    }
  }

  if (!selTrack) return;

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "  track " << i+1 << " selected" << endreq;

  TrackAttributes p(track, i);
  m_attributesOfTracks.push_back(p);

  std::sort(m_attributesOfTracks.begin(), m_attributesOfTracks.end(), TrackAttributesSortPredicate);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigTrackJetFinderTool::addTrack(const Rec::TrackParticle*& track, unsigned int i) {

  TrackAttributes p(track, i);

  float z0 = track->measuredPerigee()->parameters()[Trk::z0];
  float pT = track->pt();

  if (fabs(pT) >= m_ptMin && (fabs(z0 - m_zpv) < m_dz)) {

    if (msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << "Adding selected track " << i+1 << " for finding the track-jet direction" << endreq;  

    m_attributesOfTracks.push_back(p);

  } else {

    if (msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << "Selected track " << i+1 << " not added for finding the track-jet direction since pT " 
		      << pT << " is not >= " << m_ptMin << " and/or Delta(z) is not < " << m_dz << endreq;

  }

  std::sort(m_attributesOfTracks.begin(), m_attributesOfTracks.end(), TrackAttributesSortPredicate);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigTrackJetFinderTool::addTrack(const Trk::Track*& track, unsigned int i) {

  TrackAttributes p(track, i);

  float z0 = track->perigeeParameters()->parameters()[Trk::z0];
  float pT = track->perigeeParameters()->pT();
  
  if (fabs(pT) >= m_ptMin && (fabs(z0 - m_zpv) < m_dz)) {

    if (msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << "Adding selected track " << i+1 << " for finding the track-jet direction" << endreq;  
    
    m_attributesOfTracks.push_back(p);

  } else {

    if (msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << "Selected track " << i+1 << " not added for finding the track-jet direction since pT " 
		      << pT << " is not >= " << m_ptMin << " and/or Delta(z) is not < " << m_dz << endreq;

  }

  std::sort(m_attributesOfTracks.begin(), m_attributesOfTracks.end(), TrackAttributesSortPredicate);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigTrackJetFinderTool::inputPrimaryVertexZ(const float& zPV) {

  if (msgLvl(MSG::VERBOSE))
    msg(MSG::VERBOSE) << "Z primary vertex " << zPV << endreq;

  m_zpv = zPV;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigTrackJetFinderTool::findJet(std::vector<int>& tracks, float& jetEta, float& jetPhi) {

  tracks.clear();
  float jetPx = 0, jetPy = 0, jetPz = 0;
  float jetP = 0;
  jetEta = 0, jetPhi = 0;

  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "Number of input tracks = " << m_attributesOfTracks.size()  << endreq;

  if(m_attributesOfTracks.size() < m_mult) {
    jetEta = -99; jetPhi = -99;

    if (msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << "Seed multiplicity is below threshold " << m_mult << ". Not computing track-jet direction." << endreq;

    return;
  }

  TrackAttributes seed = m_attributesOfTracks[0];

  if(fabs(seed.Pt())<m_ptSeed) { 
    jetEta = -99; jetPhi = -99;

    if (msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << "Highest-pT seed is below threshold " << m_ptSeed << ". Not computing track-jet direction." << endreq;

    return;
  }
  
  tracks.push_back(seed.Id());
  
  jetPx += seed.Px();
  jetPy += seed.Py();
  jetPz += seed.Pz();
  jetP   = sqrt(jetPx*jetPx+jetPy*jetPy+jetPz*jetPz);
  jetEta = 0.5*log( (jetP+jetPz)/(jetP-jetPz) );
  jetPhi = phiCorr(atan2(jetPy,jetPx));

  if (msgLvl(MSG::VERBOSE))
    msg(MSG::VERBOSE) << "  jetP = " << jetP << " jetEta = " << jetEta << " jetPhi = " << jetPhi << endreq;

  for(unsigned int j=1; j < m_attributesOfTracks.size(); j++) {
    
    if (msgLvl(MSG::VERBOSE))
      msg(MSG::VERBOSE) << "Considering seed " << m_attributesOfTracks[j].Id()+1 << endreq;

    float pEta = m_attributesOfTracks[j].Eta();
    float pPhi = m_attributesOfTracks[j].Phi();
    
    float dr = deltaR(jetEta,jetPhi, pEta,pPhi);
	  
    if(dr<m_dr) {
	    
      if (msgLvl(MSG::VERBOSE))
	msg(MSG::VERBOSE) << "  deltaR matching between seed and jet direction satisfied (" << dr << ">" << m_dr << ")" << endreq;

      jetPx += m_attributesOfTracks[j].Px();
      jetPy += m_attributesOfTracks[j].Py();
      jetPz += m_attributesOfTracks[j].Pz();
      tracks.push_back(m_attributesOfTracks[j].Id());
	    
      jetP = sqrt(jetPx*jetPx+jetPy*jetPy+jetPz*jetPz);
      jetEta = 0.5*log((jetP+jetPz)/(jetP-jetPz));
      jetPhi = atan2(jetPy,jetPx);
      
      if (msgLvl(MSG::VERBOSE))
	msg(MSG::VERBOSE) << "  jetP+= " << jetP << " jetEta+= " << jetEta << " jetPhi+= " << jetPhi << endreq;
    }
  }

  //float jetPt = sqrt(jetPx*jetPx+jetPy*jetPy);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigTrackJetFinderTool::deltaR(float& eta0, float& phi0, float& eta1, float& phi1) {

  float deta = eta0 - eta1;
  float dphi = phiCorr(phiCorr(phi0) - phiCorr(phi1));

  return sqrt(deta*deta+dphi*dphi);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


float TrigTrackJetFinderTool::phiCorr(float phi){

  if(phi < -m_pi) phi += 2*m_pi;
  if(phi >  m_pi) phi -= 2*m_pi;

  return phi;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode TrigTrackJetFinderTool::finalize() {

  StatusCode sc = AlgTool::finalize(); 

  msg(MSG::INFO) << "Finalizing TrigTrackJetFinderTool" << endreq;

  return sc;
}


