/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "ALFA_RawEv/ALFA_RawDataContainer.h"
#include "ALFA_EventTPCnv/ALFA_RawDataContainerCnv_p1.h"


void ALFA_RawDataContainerCnv_p1::transToPers(const ALFA_RawDataContainer* transCont, ALFA_RawDataContainer_p1* persCont, MsgStream& log) {
  
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In ALFA_RawDataContainerCnv_p1::transToPers" << endreq;

  // Invoke vector converter from the base template
  ALFA_RawDataContainer_Cnvp1_base_t::transToPers( transCont, persCont, log );

  persCont->TimeStamp = transCont->GetTimeStamp();
  persCont->TimeStampns = transCont->GetTimeStampns();
  persCont->LumiBlock = transCont->GetLumiBlock();
  persCont->BCId = transCont->GetBCId();
  persCont->lvl1Pattern = transCont->GetLvl1Pattern_POT();
  persCont->lvl2Pattern = transCont->GetLvl2Pattern_POT();
  persCont->efPattern = transCont->GetEFPattern_POT();

 }

void ALFA_RawDataContainerCnv_p1::persToTrans(const ALFA_RawDataContainer_p1* persCont, ALFA_RawDataContainer* transCont, MsgStream& log) {


if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In  ALFA_RawDataContainerCnv_p1::persToTrans" << endreq;


// Invoke vector converter from the base template
  ALFA_RawDataContainer_Cnvp1_base_t::persToTrans( persCont, transCont, log );

  transCont->SetTimeStamp (persCont->TimeStamp);
  transCont->SetTimeStampns (persCont->TimeStampns);
  transCont->SetLumiBlock (persCont->LumiBlock);
  transCont->SetBCId (persCont->BCId);
  transCont->SetLvl1Pattern (persCont->lvl1Pattern);
  transCont->SetLvl2Pattern (persCont->lvl2Pattern);
  transCont->SetEFPattern (persCont->efPattern);

}

