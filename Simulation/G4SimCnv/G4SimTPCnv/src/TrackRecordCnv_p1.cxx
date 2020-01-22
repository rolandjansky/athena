/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackRecord/TrackRecord.h"


#include "G4SimTPCnv/TrackRecord_p1.h"
#include "G4SimTPCnv/TrackRecordCnv_p1.h"


void
TrackRecordCnv_p1::persToTrans(const TrackRecord_p1* persObj, TrackRecord* transObj, MsgStream &log) const
{
   log << MSG::DEBUG << "TrackRecordCnv_p1::persToTrans called " << endmsg;

   transObj->SetPDGCode           (persObj->PDG_code());
   transObj->SetEnergy            ((double) persObj->energy());
   transObj->SetMomentum          (CLHEP::Hep3Vector(persObj->momentumX(), persObj->momentumY(), persObj->momentumZ() ));
   transObj->SetPosition          (CLHEP::Hep3Vector(persObj->positionX(), persObj->positionY(), persObj->positionZ() ));
   transObj->SetTime              ((double) persObj->time());
   transObj->SetBarCode           (persObj->barCode());
   transObj->SetVolName           (persObj->volName());
}


void
TrackRecordCnv_p1::transToPers(const TrackRecord* transObj, TrackRecord_p1* persObj, MsgStream &log) const
{
   log << MSG::DEBUG << "TrackRecordCnv_p1::transToPers called " << endmsg;
   persObj->m_PDG_code          = transObj->GetPDGCode();
   persObj->m_energy            = (float) transObj->GetEnergy();
   CLHEP::Hep3Vector mom = transObj->GetMomentum();
   persObj->m_momentumX		= (float) mom.x();
   persObj->m_momentumY		= (float) mom.y();
   persObj->m_momentumZ		= (float) mom.z();
   CLHEP::Hep3Vector pos = transObj->GetPosition();
   persObj->m_positionX		= (float) pos.x();
   persObj->m_positionY		= (float) pos.y();
   persObj->m_positionZ		= (float) pos.z();
   persObj->m_time              = (float) transObj->GetTime();
   persObj->m_barCode           = transObj->GetBarCode();
   persObj->m_volName           = transObj->GetVolName();
}

