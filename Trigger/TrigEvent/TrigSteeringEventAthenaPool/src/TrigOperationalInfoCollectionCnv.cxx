/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigOperationalInfoCollectionCnv.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollectionCnv_p1.h"
#include "TrigSteeringEventTPCnv/TrigOperationalInfoCollectionCnv_tlp1.h"

struct TrigOperationalInfoCollectionCnv_impl
{
  TrigOperationalInfoCollectionCnv_p1 m_TPConverter;
  TrigOperationalInfoCollectionCnv_tlp1 m_TPConverter_tlp1;
};

TrigOperationalInfoCollectionCnv::TrigOperationalInfoCollectionCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigOperationalInfoCollection, TrigOperationalInfoCollection_PERS >( svcloc ),
  m_impl (new TrigOperationalInfoCollectionCnv_impl)
{}
TrigOperationalInfoCollectionCnv::~TrigOperationalInfoCollectionCnv()
{
  delete m_impl;
}

//create persistent
TrigOperationalInfoCollection_PERS* TrigOperationalInfoCollectionCnv::createPersistent(TrigOperationalInfoCollection* transObj) 
{
  MsgStream mlog(messageService(), "TrigOperationalInfoCollectionConverter" );
  mlog << MSG::DEBUG << "TrigOperationalInfoCollectionCnv::createPersistent" << endreq;

  TrigOperationalInfoCollection_PERS *persObj = m_impl->m_TPConverter_tlp1.createPersistent( transObj, mlog );
  
  return persObj;
}

//createTransient
TrigOperationalInfoCollection* TrigOperationalInfoCollectionCnv::createTransient() 
{
  MsgStream mlog(messageService(), "TrigOperationalInfoCollectionConverter" );
  mlog << MSG::DEBUG << "TrigOperationalInfoCollectionCnv::createTransient " << endreq;
  
  static pool::Guid tlp1_guid("7D5A0227-E28B-4228-83C5-22F8BBB90BBF");
  static pool::Guid p1_guid("B6C95F89-C1B9-4B9D-A533-F6F4B57BD277");
  
  TrigOperationalInfoCollection *trans_obj(0);
  
  if( compareClassGuid(tlp1_guid) ) {
    
    mlog << MSG::DEBUG << "TrigOperationalInfoCollectionCnv::reading tlp1 persistent object" << endreq;
    std::auto_ptr< TrigOperationalInfoCollection_tlp1 >   col_vect( this->poolReadObject< TrigOperationalInfoCollection_tlp1 >() );
    mlog << MSG::DEBUG << "TrigOperationalInfoCollectionCnv::reading tlp1 persistent object pointer" <<  col_vect.get() << endreq;
    trans_obj = m_impl->m_TPConverter_tlp1.createTransient( col_vect.get(), mlog );
    
  }
  else if( compareClassGuid(p1_guid) ) {
    
    mlog << MSG::DEBUG << "TrigOperationalInfoCollectionCnv::reading p1 persistent object" << endreq;
    std::auto_ptr< TrigOperationalInfoCollection_p1 >   col_vect( this->poolReadObject< TrigOperationalInfoCollection_p1 >() );
    mlog << MSG::DEBUG << "TrigOperationalInfoCollectionCnv::reading p1 persistent object pointer" <<  col_vect.get() << endreq;
    trans_obj = m_impl->m_TPConverter.createTransient( col_vect.get(), mlog );
    
  }
  else {
    
    throw std::runtime_error("Unsupported persistent version of TrigOperationalInfoCollection");
  }

  return trans_obj;

}

