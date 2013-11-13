/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "JetTagInfo/MultiSVInfoPlus.h"
#undef protected
#undef private

#include "JetTagInfoTPCnv/MultiSVInfoPlusCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"

#include "GaudiKernel/MsgStream.h"


namespace Analysis {


  void MultiSVInfoPlusCnv_p1::transToPers (const MultiSVInfoPlus *transObj,
				     MultiSVInfoPlus_p1 *persObj,
				     MsgStream &msg)
  {
    /// Do the base class first.
    persObj->m_BaseTagInfo = baseToPersistent(&m_baseTagCnv,
					      transObj,
					      msg);

    /// Now, our particular members.
    persObj->m_NGTinJet = transObj->m_NGTinJet;
    persObj->m_NGTinSvx = transObj->m_NGTinSvx;
    persObj->m_N2Tpair = transObj->m_N2Tpair;
    persObj->m_normdist = transObj->m_normdist;


    persObj->m_vtxInfo.clear();
    persObj->m_vtxInfo.reserve(transObj->m_vtxInfo.size());
    for (std::vector<MSVVtxInfo*>::const_iterator itr = transObj->m_vtxInfo.begin();
	 itr != transObj->m_vtxInfo.end();
	 ++itr) {
      persObj->m_vtxInfo.push_back(toPersistent(&m_mSVVtxInfoCnv, *itr, msg));
    }



  }


  void MultiSVInfoPlusCnv_p1::persToTrans (const MultiSVInfoPlus_p1 *persObj,
				      MultiSVInfoPlus *transObj,
				      MsgStream &msg)
  {
    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);

    /// Now, our particular members.
    transObj->m_NGTinJet = persObj->m_NGTinJet;
    transObj->m_NGTinSvx = persObj->m_NGTinSvx;
    transObj->m_N2Tpair = persObj->m_N2Tpair;
    transObj->m_normdist = persObj->m_normdist;

    transObj->m_vtxInfo.clear();
    transObj->m_vtxInfo.resize(persObj->m_vtxInfo.size());

    int index = 0;
    for (std::vector<TPObjRef>::const_iterator itr = persObj->m_vtxInfo.begin();
	 itr != persObj->m_vtxInfo.end();
	 ++itr) {
      /// allocate the memory before filling
      transObj->m_vtxInfo[index] = new MSVVtxInfo();
      fillTransFromPStore(&m_mSVVtxInfoCnv, *itr, transObj->m_vtxInfo[index], msg);
      ++index;
    }

  }

}
