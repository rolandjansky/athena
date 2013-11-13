/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SoftMuonInfo_P1_H
#define JETTAGINFOTPCNV_SoftMuonInfo_P1_H

///
/// Persitent class for the truth info
///
#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <string>

namespace Analysis {
  class SoftMuonInfoCnv_p1;

  class SoftMuonInfo_p1 {
    friend class SoftMuonInfoCnv_p1;

  private:
    /// Basic info
    TPObjRef m_BaseTagInfo;

    /// Vector of SETrackInfo objects.
    std::vector<TPObjRef> m_trackinfo;
  };
}

#endif
