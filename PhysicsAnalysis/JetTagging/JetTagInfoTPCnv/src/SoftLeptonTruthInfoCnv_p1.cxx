/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the SoftLeptonTruthInfo T/P thingy. This is really
/// about moving a vector of other objects back and forth.
///

#include "JetTagInfo/SoftLeptonTruthInfo.h"
#include "JetTagInfoTPCnv/SoftLeptonTruthInfoCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include <vector>
using std::vector;

namespace Analysis {

  void SoftLeptonTruthInfoCnv_p1::transToPers (const SoftLeptonTruthInfo *transObj,
					       SoftLeptonTruthInfo_p1 *persObj,
					       MsgStream &msg)
  {
    persObj->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv, transObj, msg);

    size_t sz = transObj->numSLTrueInfo();
    persObj->m_infoList.reserve(sz);
    for (size_t i = 0; i < sz; i++) {
      persObj->m_infoList.push_back(toPersistent(&m_sltCnv, &transObj->getSLTrueInfo(i), msg));
    }
  }

  void SoftLeptonTruthInfoCnv_p1::persToTrans (const SoftLeptonTruthInfo_p1 *persObj,
					       SoftLeptonTruthInfo *transObj,
					       MsgStream &msg)
  {
    // Clear vector.
    *transObj = SoftLeptonTruthInfo();

    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);

    for (const TPObjRef& ref : persObj->m_infoList) {
      SLTrueInfo info;
      fillTransFromPStore(&m_sltCnv, ref, &info, msg);
      transObj->addSLTrueInfo (info);
    }
  }
}
