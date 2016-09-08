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

#include "TrigTrackJetFinderTool.h"

#include "FexAlgo.h"

using namespace std;
using std::vector;



//** ----------------------------------------------------------------------------------------------------------------- **//


bool TrackAttributesSortPredicate(const TrackAttributes& lhs, const TrackAttributes& rhs) {

  return fabs(lhs.Pt()) > fabs(rhs.Pt());
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigTrackJetFinderTool::TrigTrackJetFinderTool(const std::string& t, const std::string& n) :
  HLT::FexAlgo(t),
  m_pi(3.14159265358979323846)
{
  declareProperty("DeltaR", m_dr     = 0.4);
  declareProperty("DeltaZ", m_dz     = 10.0);
  declareProperty("PtMin",  m_ptMin  = 0.5);
  declareProperty("PtJet",  m_ptJet  = 0.0);
  declareProperty("PtSeed", m_ptSeed = 2.0);
  declareProperty("Mult",   m_mult   = 1);
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigTrackJetFinderTool::~TrigTrackJetFinderTool() {}


//** ----------------------------------------------------------------------------------------------------------------- **//


StatusCode TrigTrackJetFinderTool::initialize() {
  
  msg(MSG::INFO) << "Initializing TrigTrackJetFinderTool" << endmsg;    

  if (msgLvl() <= MSG::DEBUG) {
    msg(MSG::DEBUG) << "declareProperty review:" << endmsg;
    msg(MSG::DEBUG) << " cone size (R,Z)      = " << m_dr << ", " << m_dz << endmsg; 
    msg(MSG::DEBUG) << " minimum seed pT      = " << m_ptSeed << endl;
    msg(MSG::DEBUG) << " minimum jet pT       = " << m_ptJet  << endl;
    msg(MSG::DEBUG) << " minimum track pT     = " << m_ptMin  << endl;
    msg(MSG::DEBUG) << " minimum multiplicity = " << m_mult  << endl;
  }

  return StatusCode::SUCCESS;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigTrackJetFinderTool::clear() {

  if (outputLevel <= MSG::VERBOSE)
    msg(MSG::VERBOSE) << "Clearing internal track classe" << endmsg;

  m_attributesOfTracks.clear();
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigTrackJetFinderTool::addTrack(const TrigBjetTrackInfo& track, unsigned int i) {

  TrackAttributes p(track, i);

  float z0 = track.z0();
  float pT = track.pT();

  if (fabs(pT) >= m_ptMin && (fabs(z0 - m_zpv) < m_dz)) {

    if (outputLevel <= MSG::DEBUG)
      msg(MSG::DEBUG) << "Adding selected track " << i+1 << " for finding the track-jet direction" << endmsg;  

    m_attributesOfTracks.push_back(p);

  } else {

    if (outputLevel <= MSG::DEBUG)
      msg(MSG::DEBUG) << "Selected track " << i+1 << " not added for finding the track-jet direction since pT " 
		      << pT << " is not >= " << m_ptMin << " and/or Delta(z) is not < " << m_dz << endmsg;  

  }

  std::sort(m_attributesOfTracks.begin(), m_attributesOfTracks.end(), TrackAttributesSortPredicate);
}



//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigTrackJetFinderTool::inputPrimaryVertexZ(const float& zPV) {

  if (outputLevel <= MSG::VERBOSE)
    msg(MSG::VERBOSE) << "Z primary vertex " << zPV << endmsg;

  m_zpv = zPV;
}


//** ----------------------------------------------------------------------------------------------------------------- **//


void TrigTrackJetFinderTool::findJet(std::vector<int>& tracks, float& jetEta, float& jetPhi) {

  tracks.clear();
  float jetPx = 0, jetPy = 0, jetPz = 0;
  float jetP = 0;
  jetEta = 0, jetPhi = 0;

  if (outputLevel <= MSG::DEBUG)
    msg(MSG::DEBUG) << "Number of input tracks = " << m_attributesOfTracks.size()  << endl;

  if(m_attributesOfTracks.size() < m_mult) {
    jetEta = -99; jetPhi = -99;

    if (outputLevel <= MSG::DEBUG)
      msg(MSG::DEBUG) << "Seed multiplicity is below threshold " << m_mult << ". Not computing track-jet direction." << endmsg;

    return;
  }

  TrackAttributes seed = m_attributesOfTracks[0];

  if(fabs(seed.Pt())<m_ptSeed) { 
    jetEta = -99; jetPhi = -99;

    if (outputLevel <= MSG::DEBUG)
      msg(MSG::DEBUG) << "Highest-pT seed is below threshold " << m_ptSeed << ". Not computing track-jet direction." << endmsg;

    return;
  }
  
  tracks.push_back(seed.Id());
  
  double rtmp = 1;
  float tmp = 1;

  jetPx += seed.Px();
  jetPy += seed.Py();
  jetPz += seed.Pz();
  jetP   = sqrt(jetPx*jetPx+jetPy*jetPy+jetPz*jetPz);
  rtmp = ((double) (jetP+jetPz))/ ((double)(jetP-jetPz));
  tmp = rtmp;
  jetEta = 0.5*log( tmp );
  //jetEta = 0.5*log( (jetP+jetPz)/(jetP-jetPz) );
  jetPhi = phiCorr(atan2(jetPy,jetPx));

  if (outputLevel <= MSG::VERBOSE)
    msg(MSG::VERBOSE) << "  jetP = " << jetP << " jetEta = " << jetEta << " jetPhi = " << jetPhi << endmsg;

  for(unsigned int j=1; j < m_attributesOfTracks.size(); j++) {
    
    if (outputLevel <= MSG::VERBOSE)
      msg(MSG::VERBOSE) << "Considering seed " << m_attributesOfTracks[j].Id()+1 << endmsg;

    float pEta = m_attributesOfTracks[j].Eta();
    float pPhi = m_attributesOfTracks[j].Phi();
    
    float dr = deltaR(jetEta,jetPhi, pEta,pPhi);
	  
    if(dr<m_dr) {
	    
      if (outputLevel <= MSG::VERBOSE)
	msg(MSG::VERBOSE) << "  deltaR matching between seed and jet direction satisfied (" << dr << ">" << m_dr << ")" << endmsg;

      jetPx += m_attributesOfTracks[j].Px();
      jetPy += m_attributesOfTracks[j].Py();
      jetPz += m_attributesOfTracks[j].Pz();
      tracks.push_back(m_attributesOfTracks[j].Id());
	    
      jetP = sqrt(jetPx*jetPx+jetPy*jetPy+jetPz*jetPz);
      rtmp = ((double) (jetP+jetPz))/ ((double)(jetP-jetPz));
      tmp = rtmp;
      jetEta = 0.5*log( tmp );
      //jetEta = 0.5*log((jetP+jetPz)/(jetP-jetPz));
      jetPhi = atan2(jetPy,jetPx);
      
      if (outputLevel <= MSG::VERBOSE)
	msg(MSG::VERBOSE) << "  jetP+= " << jetP << " jetEta+= " << jetEta << " jetPhi+= " << jetPhi << endmsg;
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

  msg(MSG::INFO) << "Finalizing TrigTrackJetFinderTool" << endmsg;

  return StatusCode::SUCCESS;
}


