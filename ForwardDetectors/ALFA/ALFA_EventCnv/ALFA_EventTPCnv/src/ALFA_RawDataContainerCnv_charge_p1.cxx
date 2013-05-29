/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "ALFA_EventTPCnv/ALFA_RawDataContainerCnv_charge_p1.h"


void ALFA_RawDataContainerCnv_charge_p1::transToPers(const ALFA_RawDataContainer_charge* transCont, ALFA_RawDataContainer_charge_p1* persCont, MsgStream& log) {
  
  if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In ALFA_RawDataContainerCnv_charge_p1::transToPers" << endreq;

  // Invoke vector converter from the base template
  ALFA_RawDataContainer_charge_Cnvp1_base_t::transToPers( transCont, persCont, log );

 }

void ALFA_RawDataContainerCnv_charge_p1::persToTrans(const ALFA_RawDataContainer_charge_p1* persCont, ALFA_RawDataContainer_charge* transCont, MsgStream& log) {


if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In  ALFA_RawDataContainerCnv_charge_p1::persToTrans" << endreq;


// Invoke vector converter from the base template
  ALFA_RawDataContainer_charge_Cnvp1_base_t::persToTrans( persCont, transCont, log );

}

