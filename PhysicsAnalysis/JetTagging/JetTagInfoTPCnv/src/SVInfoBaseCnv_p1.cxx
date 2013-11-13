/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Code for the converters
///

#define private public
#include "JetTagInfo/SVInfoBase.h"
#undef private
#include "JetTagInfoTPCnv/SVInfoBaseCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

static Analysis::BaseTagInfoCnv_p1 baseTagInfoCnv;

namespace Analysis {

  void SVInfoBaseCnv_p1::transToPers(const SVInfoBase* pa, SVInfoBase_p1* pb, MsgStream & msg) {
    pb->m_baseTagInfo = baseToPersistent(&m_baseTagCnv, pa, msg);
  }

  void SVInfoBaseCnv_p1::persToTrans(const SVInfoBase_p1* pa, SVInfoBase* pb, MsgStream & msg) {
    fillTransFromPStore (&m_baseTagCnv, pa->m_baseTagInfo, pb, msg);
  }

}
