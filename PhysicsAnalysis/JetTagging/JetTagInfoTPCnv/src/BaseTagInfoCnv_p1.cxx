/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Translator for the base tag info -- very simple, but common for everyone.
///

#define private public
#define protected public
#include "JetTagInfo/BaseTagInfo.h"
#undef private
#undef protected
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include <iostream>

namespace Analysis {

void BaseTagInfoCnv_p1::transToPers(const BaseTagInfo* pa, BaseTagInfo_p1* pb, MsgStream &) {
  pb->m_isValid = pa->m_isValid;
  pb->m_tagLikelihood.assign(pa->m_tagLikelihood.begin(), pa->m_tagLikelihood.end());
  pb->m_weight = pa->m_weight;
  pb->m_tagJetInfoType = pa->m_tagJetInfoType;
}

void BaseTagInfoCnv_p1::persToTrans(const BaseTagInfo_p1* pa, BaseTagInfo* pb, MsgStream &) {
  pb->m_isValid = pa->m_isValid;
  pb->m_tagLikelihood.assign (pa->m_tagLikelihood.begin(), pa->m_tagLikelihood.end());
  pb->m_weight = pa->m_weight;
  pb->m_tagJetInfoType = pa->m_tagJetInfoType;
}

}
