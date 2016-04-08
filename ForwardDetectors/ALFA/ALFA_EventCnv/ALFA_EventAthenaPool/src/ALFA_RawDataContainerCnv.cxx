/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ALFA_RawDataContainerCnv.h"

ALFA_RawDataContainer_PERS* ALFA_RawDataContainerCnv::createPersistent(ALFA_RawDataContainer* transCont) {

// std::cout << " Container, cago bello  T->P " << std::endl;

  MsgStream log(msgSvc(), "ALFA_RawDataContainerCnv");
if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In ALFA_RawDataContainerCnv::createPersistent" << endreq;

ALFA_RawDataContainerCnv_p1 TPConverter;
ALFA_RawDataContainer_PERS* persCont = TPConverter.createPersistent(transCont, log);

if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "Wrote persistent BCM RDO Container with " << persCont->size() << " entries" << endreq;
 return persCont;

}

ALFA_RawDataContainer* ALFA_RawDataContainerCnv::createTransient() {
  
  MsgStream log(msgSvc(), "ALFA_RawDataContainerCnv");
if (log.level()<=MSG::DEBUG) log << MSG::DEBUG << "In ALFA_RawDataContainerCnv::createTransient" << endreq;

// std::cout << "Container, bello cago P->T " << std::endl;

 ALFA_RawDataContainerCnv_p1 TPConverter_p1;
 ALFA_RawDataContainer* transCont(0);

static const pool::Guid p1_guid("0C023583-E3D4-4C7D-9B20-B6B2A1018D2F");

if (this->compareClassGuid(p1_guid)) {

     std::auto_ptr< ALFA_RawDataContainer_p1 > persCont(poolReadObject< ALFA_RawDataContainer_p1 >());

     transCont = TPConverter_p1.createTransient(persCont.get(), log);
  }
  else throw std::runtime_error("Unsupported persistent version of Data container");


return transCont;

 
}



