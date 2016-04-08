/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_RawDataCollection_charge.h"
#include "GaudiKernel/MsgStream.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCollectionCnv_charge_p1.h"

#include <sstream>


void ALFA_RawDataCollectionCnv_charge_p1::transToPers(const ALFA_RawDataCollection_charge* transColl, ALFA_RawDataCollection_charge_p1* persColl, MsgStream& log) {
  
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In ALFA_RawDataCollectionCnv_charge_p1::transToPers" << endreq;

  // Invoke vector converter from the base template
  ALFA_RawDataCollection_charge_Cnvp1_base_t::transToPers( transColl, persColl, log );

 
   persColl->MBId_POT  = transColl->GetMBId_POT();
   persColl->EventCount_POT  = transColl->GetEventCount_POT();
   persColl->TimeStamp_POT = transColl->GetTimeStamp_POT();
   persColl->BCId_POT = transColl->GetBCId_POT();

}

void ALFA_RawDataCollectionCnv_charge_p1::persToTrans(const ALFA_RawDataCollection_charge_p1* persColl, ALFA_RawDataCollection_charge* transColl, MsgStream& log) {

if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In  ALFA_RawDataCollectionCnv_charge_p1::persToTrans" << endreq;


// Invoke vector converter from the base template
  ALFA_RawDataCollection_charge_Cnvp1_base_t::persToTrans( persColl, transColl, log );

    transColl->SetMBId_POT        (persColl->MBId_POT);
    transColl->SetEventCount_POT  (persColl->EventCount_POT);
    transColl->SetTimeStamp_POT   (persColl->TimeStamp_POT);
    transColl->SetBCId_POT        (persColl->BCId_POT);

}

