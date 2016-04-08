/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Code to fluf and compress the ip info plus data.

#include "JetTagInfo/IPInfoPlus.h"
#include "JetTagInfoTPCnv/IPInfoPlusCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include "TPHelpers.h"

namespace Analysis {

  ///
  /// Save data to the persistant format
  ///
  void IPInfoPlusCnv_p1::transToPers(const IPInfoPlus* pa, IPInfoPlus_p1* pb, MsgStream & msg) {
    pb->m_baseTagInfo = baseToPersistent(&m_baseTagCnv, pa, msg);

#ifdef notyet
    m_trackInfoVectorCnv.transToPers(&pa->m_trackinfo,
				     &pb->m_trackinfo,
				     msg);
#endif

    size_t sz = pa->numTrackInfo();
    pb->m_trackinfo.clear();
    pb->m_trackinfo.reserve(sz);
    for (size_t i = 0; i < sz; i++) {
      pb->m_trackinfo.push_back(toPersistent(&m_trackInfoCnv, 
                                             &pa->getTrackInfo(i), msg));
    }
  }

  ///
  /// Fluf the data up again.
  ///
  void IPInfoPlusCnv_p1::persToTrans(const IPInfoPlus_p1* pa, IPInfoPlus* pb, MsgStream & msg)
  {
    /// Clear vector.
    *pb = IPInfoPlus();

    fillTransFromPStore (&m_baseTagCnv, pa->m_baseTagInfo, pb, msg);

#ifdef notyet
    m_trackInfoVectorCnv.persToTrans(&pa->m_trackinfo,
				     &pb->m_trackinfo,
				     msg);
#endif

    size_t sz = pa->m_trackinfo.size();
    for (size_t i = 0; i < sz; i++) {
      IPTrackInfo info;
      fillTransFromPStore(&m_trackInfoCnv, pa->m_trackinfo[i], &info, msg);
      pb->addTrackInfo (info);
    }
  }

}
