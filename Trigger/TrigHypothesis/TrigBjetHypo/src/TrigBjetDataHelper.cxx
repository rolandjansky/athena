/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ************************************************
//
// NAME:     DataHelper.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBjetHypo
//
// AUTHOR:   Andrea Coccaro
// EMAIL:    Andrea.Coccaro AT cern.ch
// 
// ************************************************

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "TrigBjetHypo/TrigBjetDataHelper.h"
#include <iostream>

#include <sstream>


TrigBjetTrackInfo::TrigBjetTrackInfo() :
  m_pT (0.), 
  m_eta(0.),
  m_phi(0.),
  m_d0 (0.),
  m_z0 (0.),
  m_ed0(0.),
  m_ez0(0.),
  m_d0Corr (0.),
  m_z0Corr (0.),
  //m_dummyPix(0), m_dummySct(0),
  m_pixHits(0.),
  m_sctHits(0.),
  m_siHits (0.),
  m_chi2(0.0),
  m_Blayer(0),
  m_grade(0)
{}

#ifndef VALIDATION_TOOL

TrigBjetTrackInfo::TrigBjetTrackInfo(const TrigInDetTrack*& track) :
  m_pT  (track->param()->pT()),
  m_eta (track->param()->eta()),
  m_phi (track->param()->phi0()),
  m_d0  (track->param()->a0()),
  m_z0  (track->param()->z0()),
  m_ed0 (track->param()->ea0()),
  m_ez0 (track->param()->ez0()),
  m_d0Corr (-1),
  m_z0Corr (-1),
  //m_dummyPix(0), m_dummySct(0),
  m_pixHits(-1),
  m_sctHits(-1),
  m_siHits (track->NPixelSpacePoints()+track->NSCT_SpacePoints()),
  m_chi2(0.0),
  m_Blayer(0),
  m_grade(0)
{}

TrigBjetTrackInfo::TrigBjetTrackInfo(const xAOD::TrackParticle*& track) :
  m_pT  (sin(track->theta())/track->qOverP()),
  m_eta (track->eta()),
  m_phi (track->phi()),
  m_d0  (track->d0()),
  m_z0  (track->z0()),
  m_ed0 (Amg::error(track->definingParametersCovMatrix(), 0)),
  m_ez0 (Amg::error(track->definingParametersCovMatrix(), 1)),
  m_d0Corr (-1),
  m_z0Corr (-1),
  //m_dummyPix( track->summaryValue( m_pixHits, xAOD::numberOfPixelHits) ),
  //m_dummySct( track->summaryValue( m_sctHits, xAOD::numberOfSCTHits  ) ),
  m_pixHits(-1),
  m_sctHits(-1),
  m_siHits (m_pixHits+m_sctHits),
  m_chi2(0.0),
  m_Blayer(0),
  m_grade(0)
{}

#endif

TrigBjetTrackInfo::~TrigBjetTrackInfo(){}

#ifndef VALIDATION_TOOL

void TrigBjetTrackInfo::addTrack(const TrigInDetTrack*& track) {
  m_pT  = track->param()->pT();
  m_eta = track->param()->eta();
  m_phi = track->param()->phi0();
  m_d0  = track->param()->a0();
  m_z0  = track->param()->z0();
  m_ed0 = track->param()->ea0();
  m_ez0 = track->param()->ez0();
  m_d0Corr  = -1;
  m_z0Corr  = -1;
  m_pixHits = track->NPixelSpacePoints();
  m_sctHits = track->NSCT_SpacePoints();
  m_siHits  = track->NPixelSpacePoints()+track->NSCT_SpacePoints();
}

void TrigBjetTrackInfo::addTrack(const xAOD::TrackParticle*& track) {
  m_pT  = sin(track->theta())/track->qOverP();
  m_eta = track->eta();
  m_phi = track->phi();
  m_d0  = track->d0();
  m_z0  = track->z0();
  m_ed0 = Amg::error(track->definingParametersCovMatrix(), 0);
  m_ez0 = Amg::error(track->definingParametersCovMatrix(), 1);
  m_d0Corr  = -1;
  m_z0Corr  = -1;
  uint8_t nPixHits    = 0;  
  uint8_t nSCTHits    = 0; 
  track->summaryValue(nPixHits, xAOD::numberOfPixelHits);
  track->summaryValue(nSCTHits, xAOD::numberOfSCTHits);
  m_pixHits = (int)nPixHits;
  m_sctHits = (int)nSCTHits;
  m_siHits  = m_pixHits+m_sctHits;
  
}

#endif

void TrigBjetTrackInfo::clear() {
  m_pT  = 0;
  m_eta = 0;
  m_phi = 0;
  m_d0  = 0;
  m_z0  = 0;
  m_ed0 = 0;
  m_ez0 = 0;
  m_d0Corr  = 0;
  m_z0Corr  = 0;
  m_pixHits = 0;
  m_sctHits = 0;
  m_siHits  = 0;
}

std::string str (const TrigBjetTrackInfo& obj) {

  std::stringstream ss;

  ss << "Trk obj:"
     << " pT "   << obj.pT()
     << ",eta " << obj.eta()
     << ",phi " << obj.phi()
     << ",d0 "  << obj.d0()
     << ",z0 "  << obj.z0()
     << ",ed0 " << obj.ed0()
     << ",ez0 " << obj.ez0()
     << ",d0C " << obj.d0Corr()
     << ",z0C " << obj.z0Corr()
     << ",Pix " << obj.pixHits()
     << ",SCT " << obj.sctHits()
     << ",Si "  << obj.siHits();

  return ss.str();
}
  
std::ostream& operator<< (std::ostream& m, const TrigBjetTrackInfo& obj) {

  return (m << str(obj));
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetPrmVtxInfo::TrigBjetPrmVtxInfo() : 
  m_xBS(0.),
  m_yBS(0.),
  m_zBS(0.),
  m_xPV(0.),
  m_yPV(0.),
  m_zPV(0.),
  m_xBSTilt(0.),
  m_yBSTilt(0.),
  m_xBSWidth(0.),
  m_yBSWidth(0.),
  m_zBSWidth(0.),
  m_beamSpotStatus(0.)
{}

TrigBjetPrmVtxInfo::~TrigBjetPrmVtxInfo() {}

void TrigBjetPrmVtxInfo::clear() {
  m_xBS            = 0;
  m_yBS            = 0;
  m_zBS            = 0;
  m_xPV            = 0;
  m_yPV            = 0;
  m_zPV            = 0;
  m_xBSTilt        = 0;
  m_yBSTilt        = 0;
  m_xBSWidth       = 0;
  m_yBSWidth       = 0;
  m_zBSWidth       = 0;
  m_beamSpotStatus = 0;
}

std::string str (const TrigBjetPrmVtxInfo& obj) {

  std::stringstream ss;

  ss << "PrmVtx obj:"
     << " Beam spot (" << obj.xBeamSpot()
     << ","            << obj.yBeamSpot()
     << ","            << obj.zBeamSpot()
     << ") Tilt ("      << obj.xBeamSpotTilt()
     << ","            << obj.yBeamSpotTilt()
     << ",0) Width ("     << obj.xBeamSpotWidth()
     << ","            << obj.yBeamSpotWidth()
     << ","            << obj.zBeamSpotWidth()
     << ") Status "     << obj.beamSpotStatus()
     << " PrmVtx ("    << obj.xPrmVtx()
     << ","            << obj.yPrmVtx()
     << ","            << obj.zPrmVtx()
     << ")" ;

  return ss.str();
}
  
std::ostream& operator<< (std::ostream& m, const TrigBjetPrmVtxInfo& obj) {

  return (m << str(obj));
}


//** ----------------------------------------------------------------------------------------------------------------- **//


TrigBjetSecVtxInfo::TrigBjetSecVtxInfo() : 
  m_vtxMass(0.),
  m_eFrac  (0.),
  m_sign3D (0.),
  m_nTracks(0), 
  m_n2TrkVtx(0),
  m_isValid(false)
{}

TrigBjetSecVtxInfo::TrigBjetSecVtxInfo(float vtxMass, float eFrac, float sign3D, int nTracks, int n2TrkVtx) : 
  m_vtxMass(vtxMass),
  m_eFrac  (eFrac  ),
  m_sign3D (sign3D ),
  m_nTracks(nTracks), 
  m_n2TrkVtx(n2TrkVtx),
  m_isValid(false)
{}

TrigBjetSecVtxInfo::~TrigBjetSecVtxInfo() {}

void TrigBjetSecVtxInfo::clear() {
  m_vtxMass = 0;
  m_eFrac   = 0;
  m_sign3D  = 0;
  m_nTracks = 0;
  m_n2TrkVtx = 0;
  m_isValid = false;
}

std::string str (const TrigBjetSecVtxInfo& obj) {

  std::stringstream ss;

  ss << "SecVtx obj:"
     << "vertex mass "                << obj.vtxMass()
     << ",decay length significance " << obj.decayLengthSignificance()
     << ",energy fraction "           << obj.energyFraction()
     << ",track multiplicity "        << obj.nTrksInVtx()
     << ",2-track vertex multiplicity "        << obj.n2TrkVtx();

  return ss.str();
}
  
std::ostream& operator<< (std::ostream& m, const TrigBjetSecVtxInfo& obj) {

  return (m << str(obj));
}


//** ----------------------------------------------------------------------------------------------------------------- **//

 
TrigBjetJetInfo::TrigBjetJetInfo() : 
  m_etaRoI   (0.),
  m_phiRoI   (0.),
  m_etaTrkJet(0.),
  m_phiTrkJet(0.),
  m_etaJet   (0.),
  m_phiJet   (0.),
  m_etJet    (0.)
{}

TrigBjetJetInfo::~TrigBjetJetInfo() {}

void TrigBjetJetInfo::clear() {
  m_etaRoI    = 0;
  m_phiRoI    = 0;
  m_etaTrkJet = 0;
  m_phiTrkJet = 0;
  m_etaJet    = 0;
  m_phiJet    = 0;
  m_etJet     = 0;
}

std::string str (const TrigBjetJetInfo& obj) {

  std::stringstream ss;

  ss << "Jet obj:"
     << " eta RoI "    << obj.etaRoI()
     << ",phi RoI "    << obj.phiRoI()
     << ",eta TrkJet " << obj.etaTrkJet()
     << ",phi TrkJet " << obj.phiTrkJet()
     << ",eta Jet "    << obj.etaJet()
     << ",phi Jet "    << obj.phiJet()
     << ",et Jet "     << obj.etJet();

  return ss.str();
}
  
std::ostream& operator<< (std::ostream& m, const TrigBjetJetInfo& obj) {

  return (m << str(obj));
}

