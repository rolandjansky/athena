/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKPool/FTKTrackFitterStats.h"

#include "FTKTrackFitterStatsCnv_p1.h"

void FTKTrackFitterStatsCnv_p1::persToTrans(const FTKTrackFitterStats_p1 *persObj, 
				    FTKTrackFitterStats *transObj, MsgStream&) const
{
  transObj->setNCombs                 (persObj->m_ncombs);
  transObj->setNFits                  (persObj->m_nfits);
  transObj->setNFitsMajority          (persObj->m_nfits_maj);
  transObj->setNFitsMajority_pix       (persObj->m_nfits_maj_pix);
  transObj->setNFitsMajority_SCT      (persObj->m_nfits_maj_SCT);
  transObj->setNFitsRecovery          (persObj->m_nfits_rec);
  transObj->setNAddFitsRecovery       (persObj->m_nfits_addrec);
  transObj->setNFitsBad               (persObj->m_nfits_bad);
  transObj->setNFitsHWRejected        (persObj->m_nfits_rej);
  transObj->setNFitsBadMajority       (persObj->m_nfits_badmaj);
  transObj->setNFitsHWRejectedMajority (persObj->m_nfits_rejmaj);
}


void FTKTrackFitterStatsCnv_p1::transToPers(const FTKTrackFitterStats *transObj,
				    FTKTrackFitterStats_p1 *persObj, MsgStream&) const
{
  persObj->m_ncombs = transObj->getNCombs();
  persObj->m_nfits = transObj->getNFits();
  persObj->m_nfits_maj = transObj->getNFitsMajority();
  persObj->m_nfits_maj_pix = transObj->getNFitsMajority_pix();
  persObj->m_nfits_maj_SCT = transObj->getNFitsMajority_SCT();
  persObj->m_nfits_rec = transObj->getNFitsRecovery();
  persObj->m_nfits_addrec = transObj->getNAddFitsRecovery();
  persObj->m_nfits_bad = transObj->getNFitsBad();
  persObj->m_nfits_rej = transObj->getNFitsHWRejected();
  persObj->m_nfits_badmaj = transObj->getNFitsBadMajority();
  persObj->m_nfits_rejmaj = transObj->getNFitsHWRejectedMajority();
}
