/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_DigitContainerCnv.h"

LUCID_DigitContainer_PERS* LUCID_DigitContainerCnv::createPersistent(LUCID_DigitContainer* transCont) {

  MsgStream mlog(msgSvc(), "LUCID_DigitContainerCnv");

  mlog << MSG::DEBUG << "In LUCID_DigitContainerCnv::createPersistent " << endreq;
  
  LUCID_DigitContainerCnv_p2 converter;
  LUCID_DigitContainer_PERS* persObj = converter.createPersistent(transCont, mlog);
  
  return persObj;
}

LUCID_DigitContainer* LUCID_DigitContainerCnv::createTransient() {
  
  MsgStream mlog(msgSvc(), "LUCID_DigitContainer");

  mlog << MSG::DEBUG << "In LUCID_DigitContainerCnv::createTransient " << endreq;
  
  LUCID_DigitContainerCnv_p2 converter_p2;
  
  LUCID_DigitContainer* trans_cont(0);
  
  static const pool::Guid p2_guid ("08BBB3C5-BCAE-4540-8F3A-76D873045585");
  static const pool::Guid p1_guid ("681CDB46-D6D0-4C59-98C9-398B43873B50");
  static const pool::Guid old_guid("49672224-A5AB-4D4C-802E-CA0749038ECC");
  
  if (this->compareClassGuid(p2_guid)) {
    
    std::auto_ptr< LUCID_DigitContainer_p2 > col_vect( this->poolReadObject< LUCID_DigitContainer_p2 >());
    
    trans_cont = converter_p2.createTransient(col_vect.get(), mlog);
  }
  else if(this->compareClassGuid(p1_guid)) {
    
    trans_cont = this->poolReadObject<LUCID_DigitContainer>();
  } 
  else if(this->compareClassGuid(old_guid)) {
    
    trans_cont = this->poolReadObject<LUCID_DigitContainer>();
  } 
  else throw std::runtime_error("Unsupported persistent version of Data container");

  return trans_cont;
}



