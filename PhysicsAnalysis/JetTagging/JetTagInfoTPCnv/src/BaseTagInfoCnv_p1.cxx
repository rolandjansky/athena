/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Translator for the base tag info -- very simple, but common for everyone.
///

#include "JetTagInfo/BaseTagInfo.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include <iostream>

namespace Analysis {

void BaseTagInfoCnv_p1::transToPers(const BaseTagInfo* pa, BaseTagInfo_p1* pb, MsgStream &) {
  pb->m_isValid = pa->isValid();
  pb->m_tagLikelihood.assign(pa->tagLikelihood().begin(), pa->tagLikelihood().end());
  pb->m_weight = pa->weight();
  pb->m_tagJetInfoType = pa->infoType();
}

void BaseTagInfoCnv_p1::persToTrans(const BaseTagInfo_p1* pa, BaseTagInfo* pb, MsgStream &) {
  pb->setValid (pa->m_isValid);
  std::vector<double> tagLikelihood (pa->m_tagLikelihood.begin(), pa->m_tagLikelihood.end());
  pb->setTagLikelihood (std::move (tagLikelihood));
  pb->setWeight (pa->m_weight);
  pb->setTagJetInfoType (pa->m_tagJetInfoType);
}

}
