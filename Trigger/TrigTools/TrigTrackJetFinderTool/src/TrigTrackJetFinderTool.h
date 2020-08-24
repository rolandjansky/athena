/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *********************************************************
//
// NAME:     TrigTrackJetFinderTool.h
// PACKAGE:  Trigger/TrigTools/TrigTrackJetFinderTool
//
// AUTHOR:   Andrea Coccaro    (Andrea.Coccaro@ge.infn.it)
//           Andrew Lowe       (Andrew.Lowe@cern.ch)
//           Ariel Schwartzman (sch@slac.stanford.edu)
// 
// *********************************************************

#ifndef TRIGTRACKJETFINDERTOOL_H
#define TRIGTRACKJETFINDERTOOL_H

#include <vector>
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigInDetToolInterfaces/ITrigTrackJetFinderTool.h"

#include "TrigInDetEvent/TrigInDetTrack.h"
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"



class TrackAttributes {

 public:
  
  TrackAttributes(const TrigInDetTrack*& track, unsigned int id) {

    const float pi = 3.14159265358979323846;

    m_id  = id;
    m_pt  = track->param()->pT();
    m_phi = track->param()->phi0();
    m_eta = track->param()->eta();
    m_z0  = track->param()->z0();
    m_d0  = track->param()->a0();

    if(m_phi < -pi) m_phi += 2 * pi;
    if(m_phi >  pi) m_phi -= 2 * pi;

    m_px = fabs(m_pt)*cos(m_phi);
    m_py = fabs(m_pt)*sin(m_phi);
    m_pz = fabs(m_pt)*sinh(m_eta);

  };

  TrackAttributes(const Rec::TrackParticle*& track, unsigned int id) {

    const float pi = 3.14159265358979323846;

    m_id  = id;
    m_pt  = track->pt();
    m_phi = track->phi();
    m_eta = track->eta();
    m_z0  = track->measuredPerigee()->parameters()[Trk::z0];
    m_d0  = track->measuredPerigee()->parameters()[Trk::d0];

    if(m_phi < -pi) m_phi += 2 * pi;
    if(m_phi >  pi) m_phi -= 2 * pi;

    m_px = fabs(m_pt)*cos(m_phi);
    m_py = fabs(m_pt)*sin(m_phi);
    m_pz = fabs(m_pt)*sinh(m_eta);

  };

  TrackAttributes(const Trk::Track*& track, unsigned int id) {

    const float pi = 3.14159265358979323846;

    m_id  = id;
    m_pt  = track->perigeeParameters()->pT();
    m_phi = track->perigeeParameters()->parameters()[Trk::phi];
    m_eta = track->perigeeParameters()->eta();
    m_z0  = track->perigeeParameters()->parameters()[Trk::z0];
    m_d0  = track->perigeeParameters()->parameters()[Trk::d0];

    if(m_phi < -pi) m_phi += 2 * pi;
    if(m_phi >  pi) m_phi -= 2 * pi;

    m_px = fabs(m_pt)*cos(m_phi);
    m_py = fabs(m_pt)*sin(m_phi);
    m_pz = fabs(m_pt)*sinh(m_eta);

  };

  ~TrackAttributes() {};

  int   Id()  const { return m_id; }

  float Px()  const { return m_px;  }
  float Py()  const { return m_py;  }
  float Pz()  const { return m_pz;  }
  float Pt()  const { return m_pt;  }
  float Eta() const { return m_eta; }
  float Phi() const { return m_phi; }
  float Z()   const { return m_z0;  }

  bool operator== (const TrackAttributes &other) const {
    if(this->Phi() != other.Phi()) return false;
    return true;
  }
  
 private:

  unsigned int m_id;

  float m_px;
  float m_py;
  float m_pz;
  float m_pt;
  float m_eta;
  float m_phi;
  float m_z0;
  float m_d0;

};


//** ----------------------------------------------------------------------------------------------------------------- **//


class TrigTrackJetFinderTool: public AthAlgTool, virtual public ITrigTrackJetFinderTool {

 public:

  TrigTrackJetFinderTool(const std::string&, const std::string&, const IInterface*);
  virtual ~TrigTrackJetFinderTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  void setDefTrackSel(bool doTrackSel) { m_defTrackSel=doTrackSel; }
  void setExtTrackSel(bool doTrackSel) { m_extTrackSel=doTrackSel; }

  void inputPrimaryVertexZ(const float& zPV);
  void addTrack(const TrigInDetTrack*&, unsigned int);
  void addTrack(const Rec::TrackParticle*&, unsigned int);
  void addTrack(const Trk::Track*&, unsigned int);

  void findJet(std::vector<int>& tracks, float& eta, float& phi);

  void clear();
  
 private:

  bool m_defTrackSel;
  bool m_extTrackSel;

  float m_trkSelChi2;
  int   m_trkSelBLayer;
  int   m_trkSelSiHits;
  float m_trkSelPt;

  float phiCorr(float);
  float deltaR(float& eta0, float& phi0, float& eta1, float& phi1); 

  float m_dr, m_dz, m_ptMin, m_ptJet, m_ptSeed, m_mult;
  float m_zpv;

  /** @brief Value of pi set in CTOR to the exact value in TMath.h to avoid including everything in the TMath namespace and multiple calls to TMath::Pi(). */
  const float m_pi;  

  std::vector<TrackAttributes> m_attributesOfTracks;
};

#endif

