/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SVInfoBaseInfo_P1_H
#define JETTAGINFOTPCNV_SVInfoBaseInfo_P1_H

///
/// Persitent class for the truth info
///
#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"

#include <string>

namespace Analysis {
  class SVInfoPlusCnv_p1;

  class SVInfoPlus_p1 {
    friend class SVInfoPlusCnv_p1;

  private:
    /// Basic info
    TPObjRef m_BaseTagInfo;

    /// All of this data will be written out.
    // Points to a Trk::RecVertex
    TPObjRef m_recsvx;

    long int       m_NGTinJet;
    long int       m_NGTinSvx;
    long int       m_N2Tpair;
    float          m_masssvx;
    float          m_efracsvx;
    float          m_normdist;

    // Points to SVTrackInfo list - which is just some element pointers.
    ElementLinkIntVector_p1 m_trackinfo;
  };
}

#endif
