/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Code for the converters
///

#include "JetTagInfo/IPInfoBase.h"
#include "JetTagInfoTPCnv/IPInfoBaseCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

static Analysis::BaseTagInfoCnv_p1 baseTagInfoCnv;

namespace Analysis {

void IPInfoBaseCnv_p1::transToPers(const IPInfoBase* pa, IPInfoBase_p1* pb, MsgStream & msg) {
  pb->m_baseTagInfo = baseToPersistent(&m_baseTagCnv, pa, msg);
  pb->m_ntrk = pa->nbTracks();
}

void IPInfoBaseCnv_p1::persToTrans(const IPInfoBase_p1* pa, IPInfoBase* pb, MsgStream & msg) {
  fillTransFromPStore (&m_baseTagCnv, pa->m_baseTagInfo, pb, msg);
  pb->nbTracks (pa->m_ntrk);
}

}
