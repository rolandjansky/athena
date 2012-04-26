/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigFTKPool/FTKAthTrack.h"
#undef private
#undef protected

#include "FTKAthTrackCnv_p1.h"

void FTKAthTrackCnv_p1::persToTrans(const FTKAthTrack_p1 *persObj, 
				    FTKAthTrack *transObj, MsgStream&)
{
  transObj->m_bankID = persObj->m_bankID;
  transObj->m_roadID = persObj->m_roadID;
  transObj->m_patternID = persObj->m_patternID;
  transObj->m_sectorID = persObj->m_sectorID;
  transObj->m_trackID = persObj->m_trackID;
  transObj->m_invpt = persObj->m_invpt;
  transObj->m_d0 = persObj->m_d0;
  transObj->m_phi = persObj->m_phi;
  transObj->m_z0 = persObj->m_z0;
  transObj->m_ctheta = persObj->m_ctheta;
  transObj->m_chi2 = persObj->m_chi2;
  transObj->m_origchi2 = persObj->m_origchi2;
  transObj->m_nmissing = persObj->m_nmissing;
  transObj->m_typemask = persObj->m_typemask;
  transObj->m_bitmask = persObj->m_bitmask;
  transObj->m_ncoords = persObj->m_coord.size();
  transObj->m_coord = new float[transObj->m_ncoords];
  for (int ic=0;ic!=transObj->m_ncoords;++ic) 
    transObj->m_coord[ic] = persObj->m_coord[ic];
  transObj->m_HF_rejected = persObj->m_HF_rejected;
  transObj->m_HW_rejected = persObj->m_HW_rejected;
  transObj->m_HW_track = persObj->m_HW_track;
  transObj->m_eventindex = persObj->m_eventindex;
  transObj->m_barcode = persObj->m_barcode;
  transObj->m_barcode_frac = persObj->m_barcode_frac;
}


void FTKAthTrackCnv_p1::transToPers(const FTKAthTrack *transObj,
				    FTKAthTrack_p1 *persObj, MsgStream&)
{
  persObj->m_bankID = transObj->m_bankID;
  persObj->m_roadID = transObj->m_roadID;
  persObj->m_patternID = transObj->m_patternID;
  persObj->m_sectorID = transObj->m_sectorID;
  persObj->m_trackID = transObj->m_trackID;
  persObj->m_invpt = transObj->m_invpt;
  persObj->m_d0 = transObj->m_d0;
  persObj->m_phi = transObj->m_phi;
  persObj->m_z0 = transObj->m_z0;
  persObj->m_ctheta = transObj->m_ctheta;
  persObj->m_chi2 = transObj->m_chi2;
  persObj->m_origchi2 = transObj->m_origchi2;
  persObj->m_nmissing = transObj->m_nmissing;
  persObj->m_typemask = transObj->m_typemask;
  persObj->m_bitmask = transObj->m_bitmask;
  persObj->m_coord.resize(transObj->m_ncoords);
  for (int ic=0;ic!=transObj->m_ncoords;++ic) 
    persObj->m_coord[ic] = transObj->m_coord[ic];
  persObj->m_HF_rejected = transObj->m_HF_rejected;
  persObj->m_HW_rejected = transObj->m_HW_rejected;
  persObj->m_HW_track = transObj->m_HW_track;
  persObj->m_eventindex = transObj->m_eventindex;
  persObj->m_barcode = transObj->m_barcode;
  persObj->m_barcode_frac = transObj->m_barcode_frac;
}
