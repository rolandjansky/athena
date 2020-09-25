/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_RawDataContainerCnv.h"

LUCID_RawDataContainer_PERS* LUCID_RawDataContainerCnv::createPersistent(LUCID_RawDataContainer* transCont) {

  MsgStream mlog(msgSvc(), "LUCID_RawDataContainerCnv");

  mlog << MSG::DEBUG << "In LUCID_RawDataContainerCnv::createPersistent " << endreq;
  
  LUCID_RawDataContainerCnv_p1 converter;
  LUCID_RawDataContainer_PERS* persObj = converter.createPersistent(transCont, mlog);
  
  return persObj;
}

LUCID_RawDataContainer* LUCID_RawDataContainerCnv::createTransient() {
  
  MsgStream mlog(msgSvc(), "LUCID_RawDataContainer");

  mlog << MSG::DEBUG << "In LUCID_RawDataContainerCnv::createTransient " << endreq;
  
  LUCID_RawDataContainerCnv_p1 converter_p1;
  
  LUCID_RawDataContainer* trans_cont(0);
  
  static const pool::Guid p0_guid("0C023583-E3D4-4C7D-9B20-B6B2A1018D2F");
  static const pool::Guid p1_guid("88937AAE-D299-4A80-8608-96EFB3068F9A");

  if (this->compareClassGuid(p0_guid)) { // Before T/P separation
    trans_cont = this->poolReadObject<LUCID_RawDataContainer>();
  } 
  else if (this->compareClassGuid(p1_guid)) { // First persistent version 
    
    std::auto_ptr< LUCID_RawDataContainer_p1 > col_vect( this->poolReadObject< LUCID_RawDataContainer_p1 >());
    
    trans_cont = converter_p1.createTransient(col_vect.get(), mlog);
  }
  else {
    throw std::runtime_error("Unsupported persistent version of Data container");
  }

  return trans_cont;
}



