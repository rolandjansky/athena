/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "AFP_RawEv/AFP_RawDataContainer.h"
#include "AFP_EventTPCnv/AFP_RawDataContainerCnv_p1.h"

void AFP_RawDataContainerCnv_p1::transToPers(const AFP_RawDataContainer* transCont, AFP_RawDataContainer_p1* persCont, MsgStream& log) {
 
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In AFP_RawDataContainerCnv_p1::transToPers" << endreq;
	
  // Invoke vector converter from the base template
  AFP_RawDataContainer_Cnvp1_base_t::transToPers( transCont, persCont, log );
	
  persCont->TimeStamp = transCont->GetTimeStamp();
  persCont->TimeStampns = transCont->GetTimeStampns();
  persCont->LumiBlock = transCont->GetLumiBlock();
  persCont->bcId = transCont->GetBCId();
  persCont->lvl1Id = transCont->lvl1Id();
  persCont->lvl1Pattern = transCont->GetLvl1Pattern_POT();
  persCont->lvl2Pattern = transCont->GetLvl2Pattern_POT();
  persCont->efPattern = transCont->GetEFPattern_POT();

 }
void AFP_RawDataContainerCnv_p1::persToTrans(const AFP_RawDataContainer_p1* persCont, AFP_RawDataContainer* transCont, MsgStream& log) {
	

if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In  AFP_RawDataContainerCnv_p1::persToTrans" << endreq;


// Invoke vector converter from the base template
  AFP_RawDataContainer_Cnvp1_base_t::persToTrans( persCont, transCont, log );

  transCont->SetTimeStamp (persCont->TimeStamp);
  transCont->SetTimeStampns (persCont->TimeStampns);
  transCont->SetLumiBlock (persCont->LumiBlock);
  transCont->SetBCId (persCont->bcId);
  transCont->SetLvl1Id(persCont->lvl1Id);
  transCont->SetLvl1Pattern (persCont->lvl1Pattern);
  transCont->SetLvl2Pattern (persCont->lvl2Pattern);
  transCont->SetEFPattern (persCont->efPattern);
}
