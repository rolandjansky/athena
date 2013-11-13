/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Code for the converters
///

#define private public
#include "JetTagInfo/JetProbInfoBase.h"
#undef private
#include "JetTagInfoTPCnv/JetProbInfoBaseCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

static Analysis::BaseTagInfoCnv_p1 baseTagInfoCnv;

namespace Analysis {

  void JetProbInfoBaseCnv_p1::transToPers(const JetProbInfoBase* pa, JetProbInfoBase_p1* pb, MsgStream & msg) {
    pb->m_baseTagInfo = baseToPersistent(&m_baseTagCnv, pa, msg);
    pb->m_ntrk = pa->m_ntrk;
  }

  void JetProbInfoBaseCnv_p1::persToTrans(const JetProbInfoBase_p1* pa, JetProbInfoBase* pb, MsgStream & msg) {
    fillTransFromPStore (&m_baseTagCnv, pa->m_baseTagInfo, pb, msg);
    pb->m_ntrk = pa->m_ntrk;
  }

}
