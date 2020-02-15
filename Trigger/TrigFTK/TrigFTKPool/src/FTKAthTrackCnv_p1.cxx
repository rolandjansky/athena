/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKPool/FTKAthTrack.h"

#include "FTKAthTrackCnv_p1.h"

void FTKAthTrackCnv_p1::persToTrans(const FTKAthTrack_p1 *persObj, 
				    FTKAthTrack *transObj, MsgStream&) const
{
  transObj->setBankID    (persObj->m_bankID);
  transObj->setRoadID    (persObj->m_roadID);
  transObj->setPatternID (persObj->m_patternID);
  transObj->setSectorID  (persObj->m_sectorID);
  transObj->setTrackID   (persObj->m_trackID);
  transObj->setHalfInvPt (persObj->m_invpt);
  transObj->setIP        (persObj->m_d0);
  transObj->setPhi       (persObj->m_phi, false);
  transObj->setZ0        (persObj->m_z0);
  transObj->setCotTheta  (persObj->m_ctheta);
  transObj->setChi2      (persObj->m_chi2);
  transObj->setOrigChi2  (persObj->m_origchi2);
  transObj->setNMissing  (persObj->m_nmissing);
  transObj->setTypeMask  (persObj->m_typemask);
  transObj->setBitmask   (persObj->m_bitmask);

  transObj->setNCoords (persObj->m_coord.size());
  for (size_t ic=0;ic!=persObj->m_coord.size();++ic)
    transObj->setCoord(ic, persObj->m_coord[ic]);

  transObj->setHFRejected  (persObj->m_HF_rejected);
  transObj->setHWRejected  (persObj->m_HW_rejected);
  transObj->setHWTrackID   (persObj->m_HW_track);
  transObj->setEventIndex  (persObj->m_eventindex);
  transObj->setBarcode     (persObj->m_barcode);
  transObj->setBarcodeFrac (persObj->m_barcode_frac);
}


void FTKAthTrackCnv_p1::transToPers(const FTKAthTrack *transObj,
				    FTKAthTrack_p1 *persObj, MsgStream&) const
{
  persObj->m_bankID = transObj->getBankID();
  persObj->m_roadID = transObj->getRoadID();
  persObj->m_patternID = transObj->getPatternID();
  persObj->m_sectorID = transObj->getSectorID();
  persObj->m_trackID = transObj->getTrackID();
  persObj->m_invpt = transObj->getHalfInvPt();
  persObj->m_d0 = transObj->getIP();
  persObj->m_phi = transObj->getPhi();
  persObj->m_z0 = transObj->getZ0();
  persObj->m_ctheta = transObj->getCotTheta();
  persObj->m_chi2 = transObj->getChi2();
  persObj->m_origchi2 = transObj->getOrigChi2();
  persObj->m_nmissing = transObj->getNMissing();
  persObj->m_typemask = transObj->getTypeMask();
  persObj->m_bitmask = transObj->getBitmask();

  persObj->m_coord.resize(transObj->getNCoords());
  for (int ic=0;ic!=transObj->getNCoords();++ic) 
    persObj->m_coord[ic] = transObj->getCoord(ic);

  persObj->m_HF_rejected = transObj->getHFRejected();
  persObj->m_HW_rejected = transObj->getHWRejected();
  persObj->m_HW_track = transObj->getHWTrackID();
  persObj->m_eventindex = transObj->getEventIndex();
  persObj->m_barcode = transObj->getBarcode();
  persObj->m_barcode_frac = transObj->getBarcodeFrac();
}
