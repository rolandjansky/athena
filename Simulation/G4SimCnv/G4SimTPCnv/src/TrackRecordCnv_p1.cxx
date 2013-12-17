/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrackRecord/TrackRecord.h"
#undef private
#undef protected


#include "G4SimTPCnv/TrackRecord_p1.h"
#include "G4SimTPCnv/TrackRecordCnv_p1.h"


void
TrackRecordCnv_p1::persToTrans(const TrackRecord_p1* persObj, TrackRecord* transObj, MsgStream &log)
{
   log << MSG::DEBUG << "TrackRecordCnv_p1::persToTrans called " << endreq;

   transObj->PDG_code           = persObj->PDG_code();
   transObj->Energy             = (double) persObj->energy();
   transObj->Momentum           = CLHEP::Hep3Vector(persObj->momentumX(), persObj->momentumY(), persObj->momentumZ() );
   transObj->Position           = CLHEP::Hep3Vector(persObj->positionX(), persObj->positionY(), persObj->positionZ() );
   transObj->Time               = (double) persObj->time();
   transObj->barCode            = persObj->barCode();
   transObj->volName            = persObj->volName();

   log << MSG::DEBUG << "TrackRecord: "
     <<  transObj->PDG_code  << ", "
     <<  transObj->Energy  << ", "
     <<  transObj->Momentum.x()  << ", " <<  transObj->Momentum.y()  << ", " <<  transObj->Momentum.z()  << ", "
     <<  transObj->Position.x()  << ", " <<  transObj->Position.y()  << ", " <<  transObj->Position.z()  << ", "
     <<  transObj->Time << ", " << transObj->barCode << ", " << transObj->volName 
     << endreq;
}


void
TrackRecordCnv_p1::transToPers(const TrackRecord* transObj, TrackRecord_p1* persObj, MsgStream &log)
{
   log << MSG::DEBUG << "TrackRecordCnv_p1::transToPers called " << endreq;
   persObj->m_PDG_code          = transObj->PDG_code;
   persObj->m_energy            = (float) transObj->Energy;
   persObj->m_momentumX		= (float) transObj->Momentum.x();
   persObj->m_momentumY		= (float) transObj->Momentum.y();
   persObj->m_momentumZ		= (float) transObj->Momentum.z();
   persObj->m_positionX		= (float) transObj->Position.x();
   persObj->m_positionY		= (float) transObj->Position.y();
   persObj->m_positionZ		= (float) transObj->Position.z();
   persObj->m_time              = (float) transObj->Time;
   persObj->m_barCode           = transObj->barCode;
   persObj->m_volName           = transObj->volName;
}

