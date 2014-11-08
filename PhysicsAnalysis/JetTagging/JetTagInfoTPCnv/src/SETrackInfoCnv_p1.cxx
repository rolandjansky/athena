/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Translator for the soft electron track info.
///

#define private public
#define protected public
#include "JetTagInfo/SETrackInfo.h"
#undef private
#undef protected
#include "JetTagInfoTPCnv/SETrackInfoCnv_p1.h"


namespace Analysis {

  void SETrackInfoCnv_p1::transToPers(const SETrackInfo* pa, SETrackInfo_p1* pb, MsgStream &msg) {
    pb->m_valD0wrtPV = pa->m_valD0wrtPV;
    pb->m_pTrel = pa->m_pTrel;
    pb->m_tagLikelihood.assign(pa->m_tagLikelihood.begin(), pa->m_tagLikelihood.end());

    m_eleElementLinkCnv.transToPers(&pa->m_electron, &pb->m_electron, msg);
    m_phoElementLinkCnv.transToPers(&pa->m_photon, &pb->m_photon, msg);
  }

  void SETrackInfoCnv_p1::persToTrans(const SETrackInfo_p1* pa, SETrackInfo* pb, MsgStream &msg) {
    pb->m_valD0wrtPV = pa->m_valD0wrtPV;
    pb->m_pTrel = pa->m_pTrel;
    pb->m_tagLikelihood.assign(pa->m_tagLikelihood.begin(), pa->m_tagLikelihood.end());

    m_eleElementLinkCnv.persToTrans(&pa->m_electron, &pb->m_electron, msg);
    m_phoElementLinkCnv.persToTrans(&pa->m_photon, &pb->m_photon, msg);
  }

}
