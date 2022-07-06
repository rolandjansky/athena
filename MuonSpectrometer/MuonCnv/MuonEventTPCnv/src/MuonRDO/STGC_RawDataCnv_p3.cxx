/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonEventTPCnv/MuonRDO/STGC_RawDataCnv_p3.h"

void Muon::STGC_RawDataCnv_p3::persToTrans( const Muon::STGC_RawData_p3 *, Muon::STGC_RawData *,MsgStream & /**log*/ ) 
{
  // Not much point calling this!
}

void Muon::STGC_RawDataCnv_p3::transToPers( const Muon::STGC_RawData *transObj, Muon::STGC_RawData_p3 *persObj, MsgStream & /**log*/ )
{
  persObj->m_id = transObj->m_id.get_identifier32().get_compact();
  persObj->m_time = transObj->tdo();
  persObj->m_charge = transObj->charge();
  persObj->m_isDead = transObj->isDead();
  // explanation on magic relBCID by pscholer, March 2022:
  // In order to know if the charge and the time are given in physical units or ADC counts, the transient MM_RawDataClass has a corresponding bolean. 
  // To save disk space, the persistent version does not have this bolean but uses the relBCID as flag. In detector data the relBCID has a range of 0-7,
  // therefore a relBCID of 9 indicates that charge and time are in physical units while smaller values of the relBCID indicate that they are in counts.
  // In case of the time being in nano seconds, the relBCID is anyhow not meaningfull while if the time is in counts it is decomposed into the tdo (time) and the relBCID.
  if (transObj->timeAndChargeInCounts()){
      persObj->m_bcTag = transObj->bcTag();
  } else {
      persObj->m_bcTag = 9;
  }
}

Muon::STGC_RawData* Muon::STGC_RawDataCnv_p3::createTransient(const Muon::STGC_RawData_p3* persObj, MsgStream& /**log*/)
{
  // explanation on magic relBCID by pscholer, March 2022:
  // In order to know if the charge and the time are given in physical units or ADC counts, the transient MM_RawDataClass has a corresponding bolean. 
  // To save disk space, the persistent version does not have this bolean but uses the relBCID as flag. In detector data the bcTag has a maximum range of 0-7,
  // therefore a bcTag of 9 indicates that charge and time are in physical units while smaller values of the relBCID indicate that they are in counts.
  // In case of the time being in nano seconds, the bcTag is anyhow not meaningfull while if the time is in counts it is decomposed into the tdo (time) and the bcTag.
  bool timeAndChargeInCounts = ((persObj->m_bcTag) != 9);
  Muon::STGC_RawData*  trans = new STGC_RawData( Identifier (persObj->m_id),
						 persObj->m_bcTag,
             static_cast<float>(persObj->m_time),//  place holder for tdo->time from calibration
						 persObj->m_time,
						 persObj->m_charge, 
						 persObj->m_isDead,
					         timeAndChargeInCounts);
 
  return trans;
}

