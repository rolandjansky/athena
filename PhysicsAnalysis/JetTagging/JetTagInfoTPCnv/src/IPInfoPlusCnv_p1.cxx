/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Code to fluf and compress the ip info plus data.

#define private public
#include "JetTagInfo/IPInfoPlus.h"
#undef private
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

    pb->m_trackinfo.clear();
    pb->m_trackinfo.reserve(pa->m_trackinfo.size());
    for (vector<IPTrackInfo>::const_iterator itr = pa->m_trackinfo.begin();
	 itr != pa->m_trackinfo.end();
	 itr ++) {
      pb->m_trackinfo.push_back(toPersistent(&m_trackInfoCnv, &(*itr), msg));
    }
  }

  ///
  /// Fluf the data up again.
  ///
  void IPInfoPlusCnv_p1::persToTrans(const IPInfoPlus_p1* pa, IPInfoPlus* pb, MsgStream & msg) {
    fillTransFromPStore (&m_baseTagCnv, pa->m_baseTagInfo, pb, msg);

#ifdef notyet
    m_trackInfoVectorCnv.persToTrans(&pa->m_trackinfo,
				     &pb->m_trackinfo,
				     msg);
#endif

    pb->m_trackinfo.clear();
    pb->m_trackinfo.resize(pa->m_trackinfo.size());

    int index = 0;
    for (vector<TPObjRef>::const_iterator itr = pa->m_trackinfo.begin();
	 itr != pa->m_trackinfo.end();
	 itr++) {
      fillTransFromPStore(&m_trackInfoCnv, *itr, &pb->m_trackinfo[index], msg);
      index ++;
    }
  }

}
