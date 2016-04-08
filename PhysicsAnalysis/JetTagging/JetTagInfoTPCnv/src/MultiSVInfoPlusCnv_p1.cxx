/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagInfo/MultiSVInfoPlus.h"
#include "JetTagInfoTPCnv/MultiSVInfoPlusCnv_p1.h"
#include "JetTagInfoTPCnv/BaseTagInfoCnv_p1.h"
#include "CxxUtils/make_unique.h"
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
    persObj->m_NGTinJet = transObj->getNGTrackInJet();
    persObj->m_NGTinSvx = transObj->getNGTrackInSvx();
    persObj->m_N2Tpair = transObj->getN2T();
    persObj->m_normdist = transObj->getNormDist();


    persObj->m_vtxInfo.clear();
    size_t sz = transObj->numVtxInfo();
    persObj->m_vtxInfo.reserve(sz);
    for (size_t i = 0; i < sz; i++) {
      persObj->m_vtxInfo.push_back(toPersistent(&m_mSVVtxInfoCnv, transObj->getVtxInfo(i), msg));
    }
  }


  void MultiSVInfoPlusCnv_p1::persToTrans (const MultiSVInfoPlus_p1 *persObj,
				      MultiSVInfoPlus *transObj,
				      MsgStream &msg)
  {
    // Clear the contained vector.
    *transObj = MultiSVInfoPlus();

    /// Do the base class first.
    fillTransFromPStore (&m_baseTagCnv, persObj->m_BaseTagInfo, transObj, msg);

    /// Now, our particular members.
    transObj->setNGTrackInJet (persObj->m_NGTinJet);
    transObj->setNGTrackInSvx (persObj->m_NGTinSvx);
    transObj->setN2T (persObj->m_N2Tpair);
    transObj->setNormDist (persObj->m_normdist);

    for (const TPObjRef& ref : persObj->m_vtxInfo) {
      /// allocate the memory before filling
      auto info = CxxUtils::make_unique<MSVVtxInfo>();
      fillTransFromPStore(&m_mSVVtxInfoCnv, ref, info.get(), msg);
      transObj->addVtxInfo (std::move(info));
    }

  }

}
