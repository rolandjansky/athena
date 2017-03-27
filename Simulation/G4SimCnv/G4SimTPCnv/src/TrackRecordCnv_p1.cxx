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
   log << MSG::DEBUG << "TrackRecordCnv_p1::persToTrans called " << endmsg;

   transObj->m_PDG_code           = persObj->PDG_code();
   transObj->m_Energy             = (double) persObj->energy();
   transObj->m_Momentum           = CLHEP::Hep3Vector(persObj->momentumX(), persObj->momentumY(), persObj->momentumZ() );
   transObj->m_Position           = CLHEP::Hep3Vector(persObj->positionX(), persObj->positionY(), persObj->positionZ() );
   transObj->m_Time               = (double) persObj->time();
   transObj->m_barCode            = persObj->barCode();
   transObj->m_volName            = persObj->volName();

   log << MSG::DEBUG << "TrackRecord: "
     <<  transObj->m_PDG_code  << ", "
     <<  transObj->m_Energy  << ", "
     <<  transObj->m_Momentum.x()  << ", " <<  transObj->m_Momentum.y()  << ", " <<  transObj->m_Momentum.z()  << ", "
     <<  transObj->m_Position.x()  << ", " <<  transObj->m_Position.y()  << ", " <<  transObj->m_Position.z()  << ", "
     <<  transObj->m_Time << ", " << transObj->m_barCode << ", " << transObj->m_volName 
     << endmsg;
}


void
TrackRecordCnv_p1::transToPers(const TrackRecord* transObj, TrackRecord_p1* persObj, MsgStream &log)
{
   log << MSG::DEBUG << "TrackRecordCnv_p1::transToPers called " << endmsg;
   persObj->m_PDG_code          = transObj->m_PDG_code;
   persObj->m_energy            = (float) transObj->m_Energy;
   persObj->m_momentumX		= (float) transObj->m_Momentum.x();
   persObj->m_momentumY		= (float) transObj->m_Momentum.y();
   persObj->m_momentumZ		= (float) transObj->m_Momentum.z();
   persObj->m_positionX		= (float) transObj->m_Position.x();
   persObj->m_positionY		= (float) transObj->m_Position.y();
   persObj->m_positionZ		= (float) transObj->m_Position.z();
   persObj->m_time              = (float) transObj->m_Time;
   persObj->m_barCode           = transObj->m_barCode;
   persObj->m_volName           = transObj->m_volName;
}

