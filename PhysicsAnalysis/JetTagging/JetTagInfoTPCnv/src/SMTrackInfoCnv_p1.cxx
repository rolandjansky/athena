/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Translator for the soft electron track info.
///

#define private public
#define protected public
#include "JetTagInfo/SMTrackInfo.h"
#undef private
#undef protected
#include "JetTagInfoTPCnv/SMTrackInfoCnv_p1.h"


namespace Analysis {

  void SMTrackInfoCnv_p1::transToPers(const SMTrackInfo* pa, SMTrackInfo_p1* pb, MsgStream &msg) {
    pb->m_valD0wrtPV = pa->m_valD0wrtPV;
    pb->m_pTrel = pa->m_pTrel;
    pb->m_tagLikelihood.assign(pa->m_tagLikelihood.begin(), pa->m_tagLikelihood.end());

    m_elementLinkCnv.transToPers(&pa->m_muon, &pb->m_muon, msg);
    
  }

  void SMTrackInfoCnv_p1::persToTrans(const SMTrackInfo_p1* pa, SMTrackInfo* pb, MsgStream &msg) {
    pb->m_valD0wrtPV = pa->m_valD0wrtPV;
    pb->m_pTrel = pa->m_pTrel;
    pb->m_tagLikelihood.assign(pa->m_tagLikelihood.begin(),pa->m_tagLikelihood.end());

    m_elementLinkCnv.persToTrans(&pa->m_muon, &pb->m_muon, msg);
  }

}
