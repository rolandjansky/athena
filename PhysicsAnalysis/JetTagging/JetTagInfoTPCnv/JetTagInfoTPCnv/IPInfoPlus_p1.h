/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGINFOTPCNV_IPINFOPLUS_P1_H
#define JETTAGINFOTPCNV_IPINFOPLUS_P1_H

#include "JetTagInfoTPCnv/BaseTagInfo_p1.h"
#include "JetTagInfoTPCnv/IPTrackInfo_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <vector>

namespace Analysis {

class IPInfoPlus_p1 { 

friend class IPInfoPlusCnv_p1;

private:
    TPObjRef m_baseTagInfo;

    /// Poitners to the IPTrackInfo_p1 objects.
    std::vector<TPObjRef> m_trackinfo;
};

}
#endif // JETTAGINFOTPCNV_IPINFOPLUS_P1
