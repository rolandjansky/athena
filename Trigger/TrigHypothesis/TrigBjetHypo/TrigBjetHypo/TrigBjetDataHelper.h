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


#ifndef TRIGBJET_DATAHELPER
#define TRIGBJET_DATAHELPER

#include <string>

#ifndef VALIDATION_TOOL
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "Particle/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODBase/IParticle.h"

#endif


class TrigBjetTrackInfo {

 public:

  TrigBjetTrackInfo();
#ifndef VALIDATION_TOOL
  TrigBjetTrackInfo(const TrigInDetTrack*&     track);
  TrigBjetTrackInfo(const xAOD::TrackParticle*& track);
#endif

  ~TrigBjetTrackInfo();

#ifndef VALIDATION_TOOL
  void addTrack(const TrigInDetTrack*&     track);
  void addTrack(const xAOD::TrackParticle*& track);
#endif

  void setPT (const float& pT)  { m_pT=pT  ; }
  void setEta(const float& eta) { m_eta=eta; }
  void setPhi(const float& phi) { m_phi=phi; }
  void setD0 (const float& d0)  { m_d0=d0  ; }
  void setZ0 (const float& z0)  { m_z0=z0  ; }

  float pT (void) const { return m_pT ; }
  float eta(void) const { return m_eta; }
  float phi(void) const { return m_phi; }
  float d0 (void) const { return m_d0 ; }
  float z0 (void) const { return m_z0 ; }

  void setIPCorr(const float& d0Corr, const float& z0Corr) { m_d0Corr=d0Corr; m_z0Corr=z0Corr; }

  float d0Corr (void) const { return m_d0Corr ; }
  float z0Corr (void) const { return m_z0Corr ; }

  void setED0 (const float& ed0) { m_ed0=ed0  ; }
  void setEZ0 (const float& ez0) { m_ez0=ez0  ; }

  float ed0(void) const { return m_ed0 ; }
  float ez0(void) const { return m_ez0 ; }

  void setPixHits(const int& pixHits) { m_pixHits=pixHits; }
  void setSCTHits(const int& sctHits) { m_sctHits=sctHits; }
  void setSiHits (const int& siHits)  { m_siHits=siHits  ; }

  float pixHits(void) const { return m_pixHits; }
  float sctHits(void) const { return m_sctHits; }
  float siHits(void)  const { return m_siHits ; }

  void setChi2(float chi2) { m_chi2 = chi2; }
  float chi2(void) const { return m_chi2; }

  void setBlayer(int blayer) { m_Blayer = blayer; }
  int Blayer(void) const { return m_Blayer; }

  void setGrade(int grade) { m_grade = grade; }
  int grade(void) const { return m_grade; }

  void clear(void);

 private:

  float m_pT, m_eta, m_phi, m_d0, m_z0;
  float m_ed0, m_ez0;
  float m_d0Corr, m_z0Corr;
  uint8_t m_pixHits, m_sctHits, m_siHits;
  float m_chi2;
  int m_Blayer;
  int m_grade;
};

std::string str(const TrigBjetTrackInfo&);

std::ostream& operator<< (std::ostream&, const TrigBjetTrackInfo&);


//** ----------------------------------------------------------------------------------------------------------------- **//


class TrigBjetPrmVtxInfo {
      
 public:

  TrigBjetPrmVtxInfo();
  TrigBjetPrmVtxInfo(float, float, float, int);
  ~TrigBjetPrmVtxInfo();

  void setBeamSpot(const float& x, const float& y, const float& z) { m_xBS=x; m_yBS=y; m_zBS=z; }
  void setPrmVtx  (const float& x, const float& y, const float& z) { m_xPV=x; m_yPV=y; m_zPV=z; }

  float xBeamSpot(void) const { return m_xBS; }
  float yBeamSpot(void) const { return m_yBS; }
  float zBeamSpot(void) const { return m_zBS; }
  float xPrmVtx  (void) const { return m_xPV; }  
  float yPrmVtx  (void) const { return m_yPV; }
  float zPrmVtx  (void) const { return m_zPV; }  

  void setBeamSpotTilt(const float& xz, const float& yz) { m_xBSTilt=xz; m_yBSTilt=yz; }

  float xBeamSpotTilt(void) const { return m_xBSTilt; }
  float yBeamSpotTilt(void) const { return m_yBSTilt; }

  void setBeamSpotWidth(const float& x, const float& y, const float& z) { m_xBSWidth=x; m_yBSWidth=y; m_zBSWidth=z; }

  float xBeamSpotWidth(void) const { return m_xBSWidth; }
  float yBeamSpotWidth(void) const { return m_yBSWidth; }
  float zBeamSpotWidth(void) const { return m_zBSWidth; }

  void setBeamSpotStatus(const int& status) { m_beamSpotStatus=status; }

  int beamSpotStatus(void) const { return m_beamSpotStatus; }

  void clear(void);

 private:

  float m_xBS, m_yBS, m_zBS;
  float m_xPV, m_yPV, m_zPV;
  float m_xBSTilt,  m_yBSTilt;
  float m_xBSWidth, m_yBSWidth, m_zBSWidth;
  int m_beamSpotStatus;
};

std::string str(const TrigBjetPrmVtxInfo&);

std::ostream& operator<< (std::ostream&, const TrigBjetPrmVtxInfo&);


//** ----------------------------------------------------------------------------------------------------------------- **//


class TrigBjetSecVtxInfo {
      
 public:

  TrigBjetSecVtxInfo();
  TrigBjetSecVtxInfo(float, float, float, int, int);
  ~TrigBjetSecVtxInfo();

  void setVtxMass(const float& vtxMass)                { m_vtxMass=vtxMass; }
  void setDecayLengthSignificance(const float& sign3D) { m_sign3D=sign3D  ; }
  void setEnergyFraction(const float& eFrac)           { m_eFrac=eFrac    ; }
  void setNTrksInVtx(const int& nTracks)               { m_nTracks=nTracks; }
  void setN2TrkVtx(const int& n2TrkVtx)                { m_n2TrkVtx=n2TrkVtx; }
  void isValid(bool v)                                  { m_isValid=v; }

  float vtxMass(void)                 const { return m_vtxMass; }
  float decayLengthSignificance(void) const { return m_sign3D ; }
  float energyFraction(void)          const { return m_eFrac  ; }
  int   nTrksInVtx(void)              const { return m_nTracks; }
  int   n2TrkVtx(void)              const { return m_n2TrkVtx; }
  bool  isValid(void)                const { return m_isValid; }
  
  void clear(void);

 private:

  float m_vtxMass, m_eFrac, m_sign3D; 
  int   m_nTracks,m_n2TrkVtx;
  bool  m_isValid;    
};

std::string str(const TrigBjetSecVtxInfo&);

std::ostream& operator<< (std::ostream&, const TrigBjetSecVtxInfo&);


//** ----------------------------------------------------------------------------------------------------------------- **//


class TrigBjetJetInfo {
      
 public:

  TrigBjetJetInfo();
  ~TrigBjetJetInfo();

  void setEtaPhiRoI(const float& eta, const float& phi) { m_etaRoI=eta; m_phiRoI=phi; }

  float etaRoI(void) const { return m_etaRoI; }
  float phiRoI(void) const { return m_phiRoI; }

  void setEtaPhiTrkJet(const float& eta, const float& phi) { m_etaTrkJet=eta; m_phiTrkJet=phi; }

  float etaTrkJet(void) const { return m_etaTrkJet; }
  float phiTrkJet(void) const { return m_phiTrkJet; }

  void setEtaPhiJet(const float& eta, const float& phi) { m_etaJet=eta; m_phiJet=phi; }

  float etaJet(void) const { return m_etaJet; }
  float phiJet(void) const { return m_phiJet; }

  void setEtJet(const float& et) { m_etJet=et; }

  float etJet(void) const { return m_etJet; }

  void clear(void);
  
 private:

  float m_etaRoI,    m_phiRoI;
  float m_etaTrkJet, m_phiTrkJet;
  float m_etaJet,    m_phiJet;
  float m_etJet;
};

std::string str(const TrigBjetJetInfo&);

std::ostream& operator<< (std::ostream&, const TrigBjetJetInfo&);

#endif
