/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SvxSummary_P1_H
#define JETTAGINFOTPCNV_SvxSummary_P1_H

///
/// Persitent class for the truth info
///
#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <string>

namespace Analysis {
  class SvxSummaryCnv_p1;

  class SvxSummary_p1 {
    friend class SvxSummaryCnv_p1;
  private:
    /// Basic info
    TPObjRef m_BaseTagInfo;

    /// All of this data will be written out.

    TPObjRef m_Svx; //Trk::RecVertex
    std::vector<float> m_Results;

    /// Pointers to Rec::TrackParticle
    std::vector<TPObjRef> m_TrkInSvx;
    std::vector<TPObjRef> m_TrkFromV0;
  };
}

#endif
