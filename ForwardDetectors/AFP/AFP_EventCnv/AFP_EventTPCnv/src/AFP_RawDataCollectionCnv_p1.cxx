/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_RawEv/AFP_RawDataCollection.h"
#include "GaudiKernel/MsgStream.h"
#include "AFP_EventTPCnv/AFP_RawDataCollectionCnv_p1.h"

#include <sstream>


void AFP_RawDataCollectionCnv_p1::transToPers(const AFP_RawDataCollection* transColl, AFP_RawDataCollection_p1* persColl, MsgStream& log) {
 
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In AFP_RawDataCollectionCnv_p1::transToPers" << endreq;

  // Invoke vector converter from the base template
  AFP_RawDataCollection_Cnvp1_base_t::transToPers( transColl, persColl, log );

	 
  persColl->L1Id  = transColl->Get_lvl1Id();
  persColl->frontend_flag  = transColl->Get_flag();
  persColl->patternId  = transColl->Get_pattern_POT();
  persColl->BCIdId  = transColl->Get_bcid();
  persColl->header_numberId  = transColl->Get_header_number_POT();
  persColl->ADC2Id  = transColl->Get_ADC2_POT();
  persColl->TrigSyncErrId = transColl->GetTrigSyncErr();

}

void AFP_RawDataCollectionCnv_p1::persToTrans(const AFP_RawDataCollection_p1* persColl, AFP_RawDataCollection* transColl, MsgStream& log) {

  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In  AFP_RawDataCollectionCnv_p1::persToTrans" << endreq;


  // Invoke vector converter from the base template
  AFP_RawDataCollection_Cnvp1_base_t::persToTrans( persColl, transColl, log );
	
  transColl->Set_lvl1Id   (persColl->L1Id);
  transColl->Set_flag  (persColl->frontend_flag);
  transColl->Set_pattern_POT  (persColl->patternId);
  transColl->Set_bcid  (persColl->BCIdId);
  transColl->Set_header  (persColl->header_numberId);
  transColl->Set_ADC2_POT  (persColl->ADC2Id);
  transColl->SetTrigSyncErr (persColl->TrigSyncErrId);
}
