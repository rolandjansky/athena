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

#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>

#include "TrigBjetHypo/TrigBjetDataHelper.h"
#include "FexAlgo.h"
#include "MsgStream.h"

class TrackAttributes {

 public:
  
  TrackAttributes(const TrigBjetTrackInfo &track, unsigned int id) {

    const float pi = 3.14159265358979323846;

    m_id  = id;
    m_pt  = track.pT();
    m_phi = track.phi();
    m_eta = track.eta();
    m_z0  = track.z0();
    m_d0  = track.d0();

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


class TrigTrackJetFinderTool : public HLT::FexAlgo {

 public:

  TrigTrackJetFinderTool(const std::string &name = "", const std::string &desc = "");
  virtual ~TrigTrackJetFinderTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  void inputPrimaryVertexZ(const float& zPV);
  void addTrack(const TrigBjetTrackInfo&, unsigned int);

  void findJet(std::vector<int>& tracks, float& eta, float& phi);

  void clear();
  
 private:

  int outputLevel;

  float phiCorr(float);
  float deltaR(float& eta0, float& phi0, float& eta1, float& phi1); 

  float m_dr, m_dz, m_ptMin, m_ptJet, m_ptSeed, m_mult;
  float m_zpv;

  /** @brief Value of pi set in CTOR to the exact value in TMath.h to avoid including everything in the TMath namespace and multiple calls to TMath::Pi(). */
  const float m_pi;  

  std::vector<TrackAttributes> m_attributesOfTracks;
};

#endif

