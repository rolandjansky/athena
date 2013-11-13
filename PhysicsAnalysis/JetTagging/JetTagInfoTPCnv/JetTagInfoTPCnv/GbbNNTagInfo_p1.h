/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_GBBNNTAGINFO_P1_H
#define JETTAGINFOTPCNV_GBBNNTAGINFO_P1_H

///
/// Persitent class for the jet fitter tag info class.
///
#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <string>

namespace Analysis {
  class GbbNNTagInfoCnv_p1;

  class GbbNNTagInfo_p1 {
    friend class GbbNNTagInfoCnv_p1;

  private:
    /// Basic info
    TPObjRef m_BaseTagInfo;

    /// All of this data will be written out.
    int m_nMatchingTracks;
    float m_trkJetWidth;
    float m_trkJetMaxDeltaR;

  };
}

#endif
