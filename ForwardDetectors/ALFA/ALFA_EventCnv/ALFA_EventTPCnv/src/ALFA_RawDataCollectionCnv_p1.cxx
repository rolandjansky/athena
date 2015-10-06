/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "ALFA_RawEv/ALFA_RawDataCollection.h"
#undef private
#undef protected

#include "GaudiKernel/MsgStream.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCollectionCnv_p1.h"

#include <sstream>


void ALFA_RawDataCollectionCnv_p1::transToPers(const ALFA_RawDataCollection* transColl, ALFA_RawDataCollection_p1* persColl, MsgStream& log) {
  
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In ALFA_RawDataCollectionCnv_p1::transToPers" << endreq;

  // Invoke vector converter from the base template
  ALFA_RawDataCollection_Cnvp1_base_t::transToPers( transColl, persColl, log );

 
  persColl->MBId_POT  = transColl->MBId_POT;
  persColl->EventCount_POT  = transColl->EventCount_POT;
  persColl->patternId  = transColl->patternId;
  persColl->scalerId  = transColl->scalerId;
  persColl->ADC1Id  = transColl->ADC1Id;
  persColl->ADC2Id  = transColl->ADC2Id;
  persColl->TrigSyncErrId = transColl->TrigSyncErrId;

}

void ALFA_RawDataCollectionCnv_p1::persToTrans(const ALFA_RawDataCollection_p1* persColl, ALFA_RawDataCollection* transColl, MsgStream& log) {

  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In  ALFA_RawDataCollectionCnv_p1::persToTrans" << endreq;


  // Invoke vector converter from the base template
  ALFA_RawDataCollection_Cnvp1_base_t::persToTrans( persColl, transColl, log );

  transColl->MBId_POT  = persColl->MBId_POT;
  transColl->EventCount_POT  = persColl->EventCount_POT;
  transColl->patternId  = persColl->patternId;
  transColl->scalerId  = persColl->scalerId;
  transColl->ADC1Id  = persColl->ADC1Id;
  transColl->ADC2Id  = persColl->ADC2Id;
  transColl->TrigSyncErrId = persColl->TrigSyncErrId;

}

