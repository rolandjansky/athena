/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Code to fluf and compress the ip info plus data.

#include "JetTagInfo/SoftMuonInfo.h"
#include "JetTagInfoTPCnv/SoftMuonInfoCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include <vector>
using std::vector;

namespace Analysis {

  ///
  /// Save data to the persistant format
  ///
  void SoftMuonInfoCnv_p1::transToPers(const SoftMuonInfo* pa, SoftMuonInfo_p1* pb, MsgStream & msg) {
    pb->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv, pa, msg);

    size_t sz = pa->numTrackInfo();
    pb->m_trackinfo.reserve(sz);
    for (size_t i = 0; i < sz; i++) {
      pb->m_trackinfo.push_back(toPersistent(&m_smTrackInfoCnv, &pa->getTrackInfo(i), msg));
    }
  }

  ///
  /// Fluf the data up again.
  ///
  void SoftMuonInfoCnv_p1::persToTrans(const SoftMuonInfo_p1* pa, SoftMuonInfo* pb, MsgStream & msg)
  {
    // Clear vector.
    *pb = SoftMuonInfo();

    fillTransFromPStore (&m_baseTagCnv, pa->m_BaseTagInfo, pb, msg);

    for (const TPObjRef& ref : pa->m_trackinfo) {
      SMTrackInfo info;
      fillTransFromPStore(&m_smTrackInfoCnv, ref, &info, msg);
      pb->addTrackInfo (info);
    }
  }
}
