/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigFTKPool/FTKTrackFitterStats.h"
#undef private
#undef protected

#include "FTKTrackFitterStatsCnv_p1.h"

void FTKTrackFitterStatsCnv_p1::persToTrans(const FTKTrackFitterStats_p1 *persObj, 
				    FTKTrackFitterStats *transObj, MsgStream&)
{
  transObj->m_ncombs = persObj->m_ncombs;
  transObj->m_nfits = persObj->m_nfits;
  transObj->m_nfits_maj = persObj->m_nfits_maj;
  transObj->m_nfits_maj_pix = persObj->m_nfits_maj_pix;
  transObj->m_nfits_maj_SCT = persObj->m_nfits_maj_SCT;
  transObj->m_nfits_rec = persObj->m_nfits_rec;
  transObj->m_nfits_addrec = persObj->m_nfits_addrec;
  transObj->m_nfits_bad = persObj->m_nfits_bad;
  transObj->m_nfits_rej = persObj->m_nfits_rej;
  transObj->m_nfits_badmaj = persObj->m_nfits_badmaj;
  transObj->m_nfits_rejmaj = persObj->m_nfits_rejmaj;
}


void FTKTrackFitterStatsCnv_p1::transToPers(const FTKTrackFitterStats *transObj,
				    FTKTrackFitterStats_p1 *persObj, MsgStream&)
{
  persObj->m_ncombs = transObj->m_ncombs;
  persObj->m_nfits = transObj->m_nfits;
  persObj->m_nfits_maj = transObj->m_nfits_maj;
  persObj->m_nfits_maj_pix = transObj->m_nfits_maj_pix;
  persObj->m_nfits_maj_SCT = transObj->m_nfits_maj_SCT;
  persObj->m_nfits_rec = transObj->m_nfits_rec;
  persObj->m_nfits_addrec = transObj->m_nfits_addrec;
  persObj->m_nfits_bad = transObj->m_nfits_bad;
  persObj->m_nfits_rej = transObj->m_nfits_rej;
  persObj->m_nfits_badmaj = transObj->m_nfits_badmaj;
  persObj->m_nfits_rejmaj = transObj->m_nfits_rejmaj;
}
