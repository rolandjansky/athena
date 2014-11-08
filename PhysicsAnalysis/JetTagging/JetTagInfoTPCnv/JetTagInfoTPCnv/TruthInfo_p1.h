/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_TruthInfo_P1_H
#define JETTAGINFOTPCNV_TruthInfo_P1_H

///
/// Persitent class for the truth info
///
#include "EventPrimitives/EventPrimitives.h"
#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <string>

namespace Analysis {
  class TruthInfoCnv_p1;

  class TruthInfo_p1 {
    friend class TruthInfoCnv_p1;
  public:
    /// Basic info
    TPObjRef m_BaseTagInfo;

    /// All of this data will be written out.
    std::string m_jetTruthLabel;
    float m_distanceToBQuark;
    float m_distanceToCQuark;
    float m_distanceToTLepton;
    int m_BHadronPdg;
    Eigen::Vector3d m_BHadronDecayVertex;
  };
}

#endif
