/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Code for the converters
///

#define private public
#include "JetTagInfo/GbbNNTagInfo.h"
#undef private
#include "JetTagInfoTPCnv/GbbNNTagInfoCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

static Analysis::BaseTagInfoCnv_p1 baseTagInfoCnv;

namespace Analysis {

  void GbbNNTagInfoCnv_p1::transToPers(const GbbNNTagInfo* pa, GbbNNTagInfo_p1* pb, MsgStream & msg) {
    pb->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv, pa, msg);

    pb->m_nMatchingTracks = pa->m_nMatchingTracks;
    pb->m_trkJetWidth = pa->m_trkJetWidth;
    pb->m_trkJetMaxDeltaR = pa->m_trkJetMaxDeltaR;
  
  }

  void GbbNNTagInfoCnv_p1::persToTrans(const GbbNNTagInfo_p1* pa, GbbNNTagInfo* pb, MsgStream & msg) {
    fillTransFromPStore (&m_baseTagCnv, pa->m_BaseTagInfo, pb, msg);

    pb->m_nMatchingTracks = pa->m_nMatchingTracks;
    pb->m_trkJetWidth = pa->m_trkJetWidth;
    pb->m_trkJetMaxDeltaR = pa->m_trkJetMaxDeltaR;
  }

}
