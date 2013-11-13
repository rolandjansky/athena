/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Type converter for the SoftLeptonTruthInfo T/P thingy. This is really
/// about moving a vector of other objects back and forth.
///

#define private public
#include "JetTagInfo/SoftLeptonTruthInfo.h"
#undef private

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
    
    persObj->m_infoList.clear();
    persObj->m_infoList.reserve(transObj->m_sltrueinfo.size());

    for (vector<SLTrueInfo>::const_iterator itr = transObj->m_sltrueinfo.begin();
	 itr != transObj->m_sltrueinfo.end();
	 itr++) {
      
      persObj->m_infoList.push_back(toPersistent(&m_sltCnv, &(*itr), msg));
    }
  }

  void SoftLeptonTruthInfoCnv_p1::persToTrans (const SoftLeptonTruthInfo_p1 *persObj,
					       SoftLeptonTruthInfo *transObj,
					       MsgStream &msg)
  {
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);
    transObj->m_sltrueinfo.clear();
    transObj->m_sltrueinfo.resize(persObj->m_infoList.size());

    int index = 0;
    for (vector<TPObjRef>::const_iterator itr = persObj->m_infoList.begin();
	 itr != persObj->m_infoList.end();
	 itr++) {

      fillTransFromPStore(&m_sltCnv, *itr, &transObj->m_sltrueinfo[index], msg);
      index++;
    }
  }
}
