/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "MuonEventTPCnv/MuonRDO/MM_RawDataCnv_p3.h"

void Muon::MM_RawDataCnv_p3::persToTrans( const Muon::MM_RawData_p3 *, Muon::MM_RawData *,MsgStream & /**log*/ ) 
{
  // Not much point calling this!
}

void Muon::MM_RawDataCnv_p3::transToPers( const Muon::MM_RawData *transObj, Muon::MM_RawData_p3 *persObj, MsgStream & /**log*/ )
{
  persObj->m_id = transObj->m_id.get_identifier32().get_compact();
  persObj->m_channel = transObj->channel();
  persObj->m_time = transObj->time();
  persObj->m_charge = transObj->charge();
  // explanation on magic relBCID by pscholer, March 2022:
  // In order to know if the charge and the time are given in physical units or ADC counts, the transient MM_RawDataClass has a corresponding bolean. 
  // To save disk space, the persistent version does not have this bolean but uses the relBCID as flag. In detector data the relBCID has a range of 0-7,
  // therefore a relBCID of 9 indicates that charge and time are in physical units while smaller values of the relBCID indicate that they are in counts.
  // In case of the time being in nano seconds, the relBCID is anyhow not meaningfull while if the time is in counts it is decomposed into the tdo (time) and the relBCID.
  if (transObj->timeAndChargeInCounts()){
      persObj->m_relBcid = transObj->relBcid();
  } else {
      persObj->m_relBcid = 9;
  }
}

Muon::MM_RawData* Muon::MM_RawDataCnv_p3::createTransient(const Muon::MM_RawData_p3* persObj, MsgStream& /**log*/)
{
  // explanation on magic relBCID by pscholer, March 2022:
  // In order to know if the charge and the time are given in physical units or ADC counts, the transient MM_RawDataClass has a corresponding bolean. 
  // To save disk space, the persistent version does not have this bolean but uses the relBCID as flag. In detector data the relBCID has a range of 0-7,
  // therefore a relBCID of 9 indicates that charge and time are in physical units while smaller values of the relBCID indicate that they are in counts.
  // In case of the time being in nano seconds, the relBCID is anyhow not meaningfull while if the time is in counts it is decomposed into the tdo (time) and the relBCID.
  bool timeAndChargeInCounts = ((persObj->m_relBcid) != 9);
  Muon::MM_RawData*  trans = new MM_RawData( Identifier (persObj->m_id),
					     persObj->m_channel,
					     persObj->m_time, 
					     persObj->m_charge,
               persObj->m_relBcid,
               timeAndChargeInCounts);
  
  return trans;
}

