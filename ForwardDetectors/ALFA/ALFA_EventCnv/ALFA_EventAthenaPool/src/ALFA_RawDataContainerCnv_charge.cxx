/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawDataContainerCnv_charge.h"

ALFA_RawDataContainer_charge_PERS* ALFA_RawDataContainerCnv_charge::createPersistent(ALFA_RawDataContainer_charge* transCont) {

  MsgStream log(msgSvc(), "ALFA_RawDataContainerCnv_charge");
if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In ALFA_RawDataContainerCnv_charge::createPersistent" << endreq;

ALFA_RawDataContainerCnv_charge_p1 TPConverter;
ALFA_RawDataContainer_charge_PERS* persCont = TPConverter.createPersistent(transCont, log);

if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "Wrote persistent BCM RDO Container with " << persCont->size() << " entries" << endreq;
 return persCont;

}

ALFA_RawDataContainer_charge* ALFA_RawDataContainerCnv_charge::createTransient() {
  
  MsgStream log(msgSvc(), "ALFA_RawDataContainerCnv_charge");
if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In ALFA_RawDataContainerCnv_charge::createTransient" << endreq;

 ALFA_RawDataContainerCnv_charge_p1 TPConverter_p1;
 ALFA_RawDataContainer_charge* transCont(0);

static const pool::Guid p1_guid("0C023583-E3D4-4C7D-9B20-B6B2A1018D2F");

if (this->compareClassGuid(p1_guid)) {

     std::auto_ptr< ALFA_RawDataContainer_charge_p1 > persCont(poolReadObject< ALFA_RawDataContainer_charge_p1 >());

     transCont = TPConverter_p1.createTransient(persCont.get(), log);
  }
  else throw std::runtime_error("Unsupported persistent version of Data container");


return transCont;

 
}



