/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  MsgStream mlog(msgSvc(), "TrigOperationalInfoCollectionConverter" );

  TrigOperationalInfoCollection_PERS *persObj = m_impl->m_TPConverter_tlp1.createPersistent( transObj, mlog );
  
  return persObj;
}

//createTransient
TrigOperationalInfoCollection* TrigOperationalInfoCollectionCnv::createTransient() 
{
  MsgStream mlog(msgSvc(), "TrigOperationalInfoCollectionConverter" );

  static const pool::Guid tlp1_guid("7D5A0227-E28B-4228-83C5-22F8BBB90BBF");
  static const pool::Guid p1_guid("B6C95F89-C1B9-4B9D-A533-F6F4B57BD277");
  
  TrigOperationalInfoCollection *trans_obj(0);
  
  if( compareClassGuid(tlp1_guid) ) {
    
    std::unique_ptr< TrigOperationalInfoCollection_tlp1 >   col_vect( this->poolReadObject< TrigOperationalInfoCollection_tlp1 >() );
    trans_obj = m_impl->m_TPConverter_tlp1.createTransient( col_vect.get(), mlog );
    
  }
  else if( compareClassGuid(p1_guid) ) {
    
    std::unique_ptr< TrigOperationalInfoCollection_p1 >   col_vect( this->poolReadObject< TrigOperationalInfoCollection_p1 >() );
    trans_obj = m_impl->m_TPConverter.createTransient( col_vect.get(), mlog );
    
  }
  else {
    
    throw std::runtime_error("Unsupported persistent version of TrigOperationalInfoCollection");
  }

  return trans_obj;

}

