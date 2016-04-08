/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Translator for the soft electron track info.
///

#include "JetTagInfo/SMTrackInfo.h"
#include "JetTagInfoTPCnv/SMTrackInfoCnv_p1.h"


namespace Analysis {

  void SMTrackInfoCnv_p1::transToPers(const SMTrackInfo* pa, SMTrackInfo_p1* pb, MsgStream &msg) {
    pb->m_valD0wrtPV = pa->d0Value();
    pb->m_pTrel = pa->pTrel();
    pb->m_tagLikelihood.assign(pa->tagLikelihood().begin(),
                               pa->tagLikelihood().end());

    m_elementLinkCnv.transToPers(&pa->muonLink(), &pb->m_muon, msg);
    
  }

  void SMTrackInfoCnv_p1::persToTrans(const SMTrackInfo_p1* pa, SMTrackInfo* pb, MsgStream &msg)
  {
    ElementLink<MuonContainer> muonLink;
    m_elementLinkCnv.persToTrans(&pa->m_muon, &muonLink, msg);

    *pb = SMTrackInfo (muonLink,
                       pa->m_valD0wrtPV,
                       pa->m_pTrel,
                       std::vector<double> (pa->m_tagLikelihood.begin(),
                                            pa->m_tagLikelihood.end()));
  }

}
