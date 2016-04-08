/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETS_P1_H
#define HIPIXELTRACKLETS_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
class HIPixelTracklets_p1 {
  friend class HIPixelTrackletsCnv_p1;
 public:
  HIPixelTracklets_p1() {
    m_eta=0;
    m_phi=0;
    m_pt=0;
    m_etaS=0;
    m_phiS=0;
  };
  //vertex position
  TPObjRef m_vrt;

  //cluster
  //TPObjRef m_cluster;
  //cluster vector
  std::vector<TPObjRef> m_clusters;
  double m_eta;
  double m_phi;
  double m_pt;
  double m_etaS;
  double m_phiS;
};

#endif
