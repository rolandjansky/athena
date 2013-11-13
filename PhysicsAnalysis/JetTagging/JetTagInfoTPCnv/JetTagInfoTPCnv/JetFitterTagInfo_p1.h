/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_JETFITTERTAGINFO_P1_H
#define JETTAGINFOTPCNV_JETFITTERTAGINFO_P1_H

///
/// Persitent class for the jet fitter tag info class.
///
#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <string>

namespace Analysis {
  class JetFitterTagInfoCnv_p1;

  class JetFitterTagInfo_p1 {
    friend class JetFitterTagInfoCnv_p1;

  private:
    /// Basic info
    TPObjRef m_BaseTagInfo;

    /// All of this data will be written out.
    int m_nVTX;
    int m_nSingleTracks;
    int m_nTracksAtVtx;
    float m_mass;
    float m_energyFraction;
    float m_significance3d;
    float m_deltaeta;
    float m_deltaphi;
  };
}

#endif
