/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Translation for the IPTrackInfoCnv object.
///

#define private public
#include "JetTagInfo/IPTrackInfo.h"
#undef private
#include "JetTagInfoTPCnv/IPTrackInfoCnv_p1.h"

namespace Analysis {

  void IPTrackInfoCnv_p1::transToPers(const IPTrackInfo* pa, IPTrackInfo_p1* pb, MsgStream & msg) {
    m_elementLinkCnv.transToPers(&pa->m_track, &pb->m_track, msg);
    pb->m_isFromV0 = pa->m_isFromV0;
    pb->m_valD0wrtPV = pa->m_valD0wrtPV;
    pb->m_sigD0wrtPV = pa->m_sigD0wrtPV;
    pb->m_valZ0wrtPV = pa->m_valZ0wrtPV;
    pb->m_sigZ0wrtPV = pa->m_sigZ0wrtPV;
    pb->m_trackWeight2D = pa->m_trackWeight2D;
    pb->m_trackWeight3D = pa->m_trackWeight3D;
    pb->m_trackProbJP = pa->m_trackProbJP;
    pb->m_trackProbJPneg = pa->m_trackProbJPneg;

    pb->m_trackGrade_int = pa->m_trackGrade.m_gradeNumber;
    pb->m_trackGrade_string = pa->m_trackGrade.m_gradeName;
  }

  void IPTrackInfoCnv_p1::persToTrans(const IPTrackInfo_p1* pa, IPTrackInfo* pb, MsgStream & msg) {
    m_elementLinkCnv.persToTrans(&pa->m_track, &pb->m_track, msg);
    pb->m_isFromV0 = pa->m_isFromV0;
    pb->m_valD0wrtPV = pa->m_valD0wrtPV;
    pb->m_sigD0wrtPV = pa->m_sigD0wrtPV;
    pb->m_valZ0wrtPV = pa->m_valZ0wrtPV;
    pb->m_sigZ0wrtPV = pa->m_sigZ0wrtPV;
    pb->m_trackWeight2D = pa->m_trackWeight2D;
    pb->m_trackWeight3D = pa->m_trackWeight3D;
    pb->m_trackProbJP = pa->m_trackProbJP;
    pb->m_trackProbJPneg = pa->m_trackProbJPneg;

    pb->m_trackGrade.m_gradeNumber = pa->m_trackGrade_int;
    pb->m_trackGrade.m_gradeName = pa->m_trackGrade_string;

  }

}
