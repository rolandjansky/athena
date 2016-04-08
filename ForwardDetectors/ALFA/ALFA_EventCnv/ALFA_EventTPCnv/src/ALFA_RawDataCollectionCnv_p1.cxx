/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawEv/ALFA_RawDataCollection.h"
#include "GaudiKernel/MsgStream.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCollectionCnv_p1.h"

#include <sstream>


void ALFA_RawDataCollectionCnv_p1::transToPers(const ALFA_RawDataCollection* transColl, ALFA_RawDataCollection_p1* persColl, MsgStream& log) {
  
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In ALFA_RawDataCollectionCnv_p1::transToPers" << endreq;

  // Invoke vector converter from the base template
  ALFA_RawDataCollection_Cnvp1_base_t::transToPers( transColl, persColl, log );

 
  persColl->MBId_POT  = transColl->GetMBId_POT();
  persColl->EventCount_POT  = transColl->GetEventCount_POT();
  persColl->patternId  = transColl->Get_pattern_POT();
  persColl->scalerId  = transColl->Get_scaler_POT();
  persColl->ADC1Id  = transColl->Get_ADC1_POT();
  persColl->ADC2Id  = transColl->Get_ADC2_POT();
  persColl->TrigSyncErrId = transColl->GetTrigSyncErr();

}

void ALFA_RawDataCollectionCnv_p1::persToTrans(const ALFA_RawDataCollection_p1* persColl, ALFA_RawDataCollection* transColl, MsgStream& log) {

  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In  ALFA_RawDataCollectionCnv_p1::persToTrans" << endreq;


  // Invoke vector converter from the base template
  ALFA_RawDataCollection_Cnvp1_base_t::persToTrans( persColl, transColl, log );

  transColl->SetMBId_POT   (persColl->MBId_POT);
  transColl->SetEventCount_POT  (persColl->EventCount_POT);
  transColl->Set_pattern_POT  (persColl->patternId);
  transColl->Set_scaler_POT  (persColl->scalerId);
  transColl->Set_ADC1_POT  (persColl->ADC1Id);
  transColl->Set_ADC2_POT  (persColl->ADC2Id);
  transColl->SetTrigSyncErr (persColl->TrigSyncErrId);
}

