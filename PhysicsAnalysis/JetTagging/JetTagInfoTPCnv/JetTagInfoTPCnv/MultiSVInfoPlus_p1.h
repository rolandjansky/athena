/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_MultiSVInfoPlus_P1_H
#define JETTAGINFOTPCNV_MultiSVInfoPlus_P1_H

///
/// Persitent class for the truth info
///
#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <string>

namespace Analysis {
  class MultiSVInfoPlusCnv_p1;

  class MultiSVInfoPlus_p1 {
    friend class MultiSVInfoPlusCnv_p1;

  private:
    /// Basic info
    TPObjRef m_BaseTagInfo;

 
    long int       m_NGTinJet;
    long int       m_NGTinSvx;
    long int       m_N2Tpair;
    float          m_normdist;

    std::vector<TPObjRef> m_vtxInfo;

  };
}

#endif
