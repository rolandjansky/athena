/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_SoftLeptonTruthInfo_P1_H
#define JETTAGINFOTPCNV_SoftLeptonTruthInfo_P1_H

#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "JetTagInfoTPCnv/SLTrueInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

namespace Analysis {

  class SoftLeptonTruthInfo_p1 {
    friend class SoftLeptonTruthInfoCnv_p1;
  public:

  private:
    TPObjRef m_BaseTagInfo;

    std::vector<TPObjRef> m_infoList;
  };
}



#endif
