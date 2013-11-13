/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SoftElectronInfo_P1_H
#define JETTAGINFOTPCNV_SoftElectronInfo_P1_H

///
/// Persitent class for the truth info
///
#include "EventPrimitives/EventPrimitives.h"
#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <string>

namespace Analysis {
  class SoftElectronInfoCnv_p1;

  class SoftElectronInfo_p1 {
    friend class SoftElectronInfoCnv_p1;

  private:
    /// Basic info
    TPObjRef m_BaseTagInfo;

    /// All of this data will be written out.
    std::vector<float> m_vectorOfTrackProb;
    float m_nTrackProb;
    /// Vector of SETrackInfo objects.
    std::vector<TPObjRef> m_trackinfo;
  };
}

#endif
