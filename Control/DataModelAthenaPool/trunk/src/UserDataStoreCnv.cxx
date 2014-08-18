/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "UserDataStoreCnv.h"


UserDataStoreCnv::UserDataStoreCnv(ISvcLocator* svcloc) : 
  T_AthenaPoolCustomCnv<UserDataStore, UserDataStore_p1 >( svcloc),
  m_guidP1("1D3E6FC1-54CD-4239-9714-9C467160F3FA"),
  mlog(messageService(), "UserDataStoreCnv")
{}


UserDataStore_p1* UserDataStoreCnv::createPersistent(UserDataStore* transObj) {

  UserDataStore_p1* pers=new UserDataStore_p1();
  m_converterP1.transToPers(*transObj,*pers,mlog);
  return pers;
}



UserDataStore* UserDataStoreCnv::createTransient () {

  UserDataStore* trans=0;

  if (compareClassGuid(m_guidP1)) {
    if (mlog.level() <= MSG::DEBUG) mlog << MSG::DEBUG << "Reading version UserDataStore_p1" << endreq;
    trans=new UserDataStore();
    UserDataStore_p1* pers=poolReadObject<UserDataStore_p1>();
    m_converterP1.persToTrans(*pers,*trans,mlog);
    if (mlog.level() <= MSG::DEBUG) mlog << MSG::DEBUG << "Done converting UserDataStore_p1 to UserDataStore" << endreq;
    delete pers;
  }
  else {
    mlog << MSG::ERROR << "Unsupported persistent version of UserDataSvc GUID=" << m_classID.toString() << endreq;
    //throw std::runtime_error("Unsupported persistent version of UserDataSvc");
  }
  return trans;
}
