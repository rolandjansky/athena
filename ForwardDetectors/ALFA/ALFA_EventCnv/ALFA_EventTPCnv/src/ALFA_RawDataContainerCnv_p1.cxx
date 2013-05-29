/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"

#define private public
#define protected public
#include "ALFA_RawEv/ALFA_RawDataContainer.h"
#undef private
#undef protected

#include "ALFA_EventTPCnv/ALFA_RawDataContainerCnv_p1.h"


void ALFA_RawDataContainerCnv_p1::transToPers(const ALFA_RawDataContainer* transCont, ALFA_RawDataContainer_p1* persCont, MsgStream& log) {
  
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In ALFA_RawDataContainerCnv_p1::transToPers" << endreq;

  // Invoke vector converter from the base template
  ALFA_RawDataContainer_Cnvp1_base_t::transToPers( transCont, persCont, log );

  persCont->TimeStamp = transCont->TimeStamp;
  persCont->TimeStampns = transCont->TimeStampns;
  persCont->LumiBlock = transCont->LumiBlock;
  persCont->BCId = transCont->BCId;
  persCont->lvl1Pattern = transCont->lvl1Pattern;
	persCont->lvl2Pattern = transCont->lvl2Pattern;
	persCont->efPattern = transCont->efPattern;

 }

void ALFA_RawDataContainerCnv_p1::persToTrans(const ALFA_RawDataContainer_p1* persCont, ALFA_RawDataContainer* transCont, MsgStream& log) {


if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In  ALFA_RawDataContainerCnv_p1::persToTrans" << endreq;


// Invoke vector converter from the base template
  ALFA_RawDataContainer_Cnvp1_base_t::persToTrans( persCont, transCont, log );

  transCont->TimeStamp = persCont->TimeStamp;
  transCont->TimeStampns = persCont->TimeStampns;
  transCont->LumiBlock = persCont->LumiBlock;
  transCont->BCId = persCont->BCId;
  transCont->lvl1Pattern = persCont->lvl1Pattern;
	transCont->lvl2Pattern = persCont->lvl2Pattern;
	transCont->efPattern = persCont->efPattern;

}

