/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_LifetimeInfo_P1_H
#define JETTAGINFOTPCNV_LifetimeInfo_P1_H

///
/// Persitent class for the truth info
///
#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <string>

namespace Analysis {
  class LifetimeInfoCnv_p1;

  class LifetimeInfo_p1 {
    friend class LifetimeInfoCnv_p1;

  private:
    /// Basic info
    TPObjRef m_BaseTagInfo;

    /// All of this data will be written out.
    std::vector<float> m_trackSIP;            //!< signed impact parameters
    std::vector<float> m_trackSignificance;   //!< significance (= signedIP/sigD0)

    std::vector<float> m_vectorOfTrackProb;   //!< vector of track probabilities (calculated but not in use)
    float m_nTrackProb;            //!< nTrack probability (calculated but not in use)

  };
}

#endif
