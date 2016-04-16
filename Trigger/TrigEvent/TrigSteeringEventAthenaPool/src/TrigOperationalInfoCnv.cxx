/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigOperationalInfoCnv.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCnv_p1.h"

TrigOperationalInfoCnv::TrigOperationalInfoCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigOperationalInfo, TrigOperationalInfo_PERS >( svcloc ),
  m_TPConverter (new TrigOperationalInfoCnv_p1)
{}
TrigOperationalInfoCnv::~TrigOperationalInfoCnv()
{
  delete m_TPConverter;
}

//create persistent
TrigOperationalInfo_PERS* TrigOperationalInfoCnv::createPersistent(TrigOperationalInfo* transObj) 
{
  MsgStream mlog(messageService(), "TrigOperationalInfoConverter" );
  mlog << MSG::DEBUG << "TrigOperationalInfoCnv_p1::createPersistent" << endreq;

  TrigOperationalInfo_PERS *persObj = m_TPConverter->createPersistent( transObj, mlog );
  
  return persObj;
}

//createTransient
TrigOperationalInfo* TrigOperationalInfoCnv::createTransient() 
{
  MsgStream mlog(messageService(), "TrigOperationalInfoConverter" );
  mlog << MSG::DEBUG << "TrigOperationalInfoCnv_p1::createTransient " << endreq;
  
  static pool::Guid p1_guid("765F0281-801B-4F5C-8C4C-5BE7E7DB5E42");

  
  TrigOperationalInfo *trans_obj(0);
  
  if( compareClassGuid(p1_guid) ) {
    
    mlog << MSG::DEBUG << "TrigOperationalInfoCnv::reading p1 persistent object" << endreq;
    std::auto_ptr< TrigOperationalInfo_p1 >   col_vect( this->poolReadObject< TrigOperationalInfo_p1 >() );
    trans_obj = m_TPConverter->createTransient( col_vect.get(), mlog );    
  } else {
    
    throw std::runtime_error("Unsupported persistent version of Data container");
  }

  return trans_obj;

}

