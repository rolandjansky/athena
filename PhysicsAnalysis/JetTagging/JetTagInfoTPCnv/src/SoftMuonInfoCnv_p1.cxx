/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Code to fluf and compress the ip info plus data.

#define private public
#include "JetTagInfo/SoftMuonInfo.h"
#undef private
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

    pb->m_trackinfo.clear();
    pb->m_trackinfo.reserve(pa->m_trackinfo.size());
    for (vector<SMTrackInfo>::const_iterator itr = pa->m_trackinfo.begin();
	 itr != pa->m_trackinfo.end();
	 itr ++) {
      pb->m_trackinfo.push_back(toPersistent(&m_smTrackInfoCnv, &(*itr), msg));
    }
  }

  ///
  /// Fluf the data up again.
  ///
  void SoftMuonInfoCnv_p1::persToTrans(const SoftMuonInfo_p1* pa, SoftMuonInfo* pb, MsgStream & msg) {
    fillTransFromPStore (&m_baseTagCnv, pa->m_BaseTagInfo, pb, msg);

    pb->m_trackinfo.clear();
    pb->m_trackinfo.resize(pa->m_trackinfo.size());

    int index = 0;
    for (vector<TPObjRef>::const_iterator itr = pa->m_trackinfo.begin();
	 itr != pa->m_trackinfo.end();
	 itr++) {
      fillTransFromPStore(&m_smTrackInfoCnv, *itr, &pb->m_trackinfo[index], msg);
      index ++;
    }

  }

}
