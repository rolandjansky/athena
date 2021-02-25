/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LUCID_SimHitCollectionCnv.h"
#include "LUCID_EventTPCnv/LUCID_SimHitCollectionCnv_p1.h"
#include "LUCID_EventTPCnv/LUCID_SimHitCollectionCnv_p2.h"
#include "LUCID_EventTPCnv/LUCID_SimHit_p1.h"
#include "LUCID_EventTPCnv/LUCID_SimHit_p2.h"


LUCID_SimHitCollection_PERS* LUCID_SimHitCollectionCnv::createPersistent(LUCID_SimHitCollection* transCont) {
  MsgStream mlog(msgSvc(), "LUCID_SimHitCollectionCnv");
  mlog << MSG::DEBUG << "In LUCID_SimHitCollectionCnv::createPersistent " << endmsg;
  LUCID_SimHitCollectionCnv_p1 converter;
  LUCID_SimHitCollection_PERS* persObj = converter.createPersistent(transCont, mlog);
  return persObj;
}


LUCID_SimHitCollection* LUCID_SimHitCollectionCnv::createTransient() {
  MsgStream mlog(msgSvc(), "LUCID_SimHitCollectionCnv");
  mlog << MSG::DEBUG << "In LUCID_SimHitCollectionCnv::createTransient " << endmsg;
  LUCID_SimHitCollectionCnv_p1 converter_p1;
  LUCID_SimHitCollectionCnv_p2 converter_p2;
  static const pool::Guid p2_guid ("149F1834-1D98-4F35-A1B3-C9AA083D6838");
  static const pool::Guid p1_guid ("9ACC906C-74CA-4F77-AC16-2A503358C2D1");
  static const pool::Guid old_guid("7BCDF079-FD96-4B18-B1E7-FA5EDDB026F2");

  LUCID_SimHitCollection* trans_cont{};
  if (this->compareClassGuid(p2_guid)) {
    std::unique_ptr< LUCID_SimHitCollection_p2 > col_vect( this->poolReadObject< LUCID_SimHitCollection_p2 >());
    trans_cont = converter_p2.createTransient(col_vect.get(), mlog);
  }
  else if(this->compareClassGuid(p1_guid)) {
    std::unique_ptr< LUCID_SimHitCollection_p1 > col_vect( this->poolReadObject< LUCID_SimHitCollection_p1 >());
    trans_cont = converter_p1.createTransient(col_vect.get(), mlog);
  }
  else if(this->compareClassGuid(old_guid)) {
    trans_cont = this->poolReadObject<LUCID_SimHitCollection>();
  }
  else {
    mlog << MSG::ERROR << "BAD GUID: " << m_i_poolToken->toString() << endmsg;
    throw std::runtime_error("Unsupported persistent version of Data container");
  }
  return trans_cont;
}
