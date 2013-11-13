/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Code for the converters
///

#define private public
#include "JetTagInfo/TrackCountingInfo.h"
#undef private
#include "JetTagInfoTPCnv/TrackCountingInfoCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

static Analysis::BaseTagInfoCnv_p1 baseTagInfoCnv;

namespace Analysis {

  void TrackCountingInfoCnv_p1::transToPers(const TrackCountingInfo* pa, 
					    TrackCountingInfo_p1* pb, 
					    MsgStream & msg) {
    pb->m_baseTagInfo = baseToPersistent(&m_baseTagCnv, pa, msg);

    pb->m_ntrk          = pa->m_ntrk;
    pb->m_d0sig_2nd     = pa->m_d0sig_2nd;
    pb->m_d0sig_abs_2nd = pa->m_d0sig_abs_2nd;
    pb->m_d0sig_3rd     = pa->m_d0sig_3rd;
    pb->m_d0sig_abs_3rd = pa->m_d0sig_abs_3rd;
  }

  void TrackCountingInfoCnv_p1::persToTrans(const TrackCountingInfo_p1* pa, 
					    TrackCountingInfo* pb, 
					    MsgStream & msg) {
    fillTransFromPStore (&m_baseTagCnv, pa->m_baseTagInfo, pb, msg);

    pb->m_ntrk          = pa->m_ntrk;
    pb->m_d0sig_2nd     = pa->m_d0sig_2nd;
    pb->m_d0sig_abs_2nd = pa->m_d0sig_abs_2nd;
    pb->m_d0sig_3rd     = pa->m_d0sig_3rd;
    pb->m_d0sig_abs_3rd = pa->m_d0sig_abs_3rd;
    
  }
}
