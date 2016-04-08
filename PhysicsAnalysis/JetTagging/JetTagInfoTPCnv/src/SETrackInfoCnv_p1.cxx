/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// Translator for the soft electron track info.
///

#include "JetTagInfo/SETrackInfo.h"
#include "JetTagInfoTPCnv/SETrackInfoCnv_p1.h"


namespace Analysis {

  void SETrackInfoCnv_p1::transToPers(const SETrackInfo* pa, SETrackInfo_p1* pb, MsgStream &msg) {
    pb->m_valD0wrtPV = pa->d0Value();
    pb->m_pTrel = pa->pTrel();
    pb->m_tagLikelihood.assign(pa->tagLikelihood().begin(),
                               pa->tagLikelihood().end());

    if (pa->isPhoton()) {
      m_eleElementLinkCnv.transToPers(&pa->electronLink(), &pb->m_electron, msg);
    }
    else {
      m_phoElementLinkCnv.transToPers(&pa->photonLink(), &pb->m_photon, msg);
    }
  }

  void SETrackInfoCnv_p1::persToTrans(const SETrackInfo_p1* pa, SETrackInfo* pb, MsgStream &msg)
  {
    if (pa->m_electron.m_SGKeyHash != 0) {
      assert (pa->m_photon.m_SGKeyHash == 0);
      ElementLink<ElectronContainer> eleLink;
      m_eleElementLinkCnv.persToTrans(&pa->m_electron, &eleLink, msg);
      *pb = SETrackInfo (eleLink,
                         pa->m_valD0wrtPV,
                         pa->m_pTrel,
                         std::vector<double> (pa->m_tagLikelihood.begin(),
                                              pa->m_tagLikelihood.end()));

    }
    else {
      ElementLink<PhotonContainer> phoLink;
      m_phoElementLinkCnv.persToTrans(&pa->m_photon, &phoLink, msg);
      *pb = SETrackInfo (phoLink,
                         pa->m_valD0wrtPV,
                         pa->m_pTrel,
                         std::vector<double> (pa->m_tagLikelihood.begin(),
                                              pa->m_tagLikelihood.end()));
    }
  }
}
