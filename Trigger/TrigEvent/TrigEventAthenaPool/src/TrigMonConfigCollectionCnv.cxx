/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigMonConfigCollectionCnv.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCollectionCnv_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCollectionCnv_tlp1.h"

class TrigMonConfigCollectionCnv_impl
{
public:
  TrigMonConfigCollectionCnv_p1 m_TPConverter;
  TrigMonConfigCollectionCnv_tlp1 m_TPConverter_tlp1;
};

//---------------------------------------------------------------------------------------------
TrigMonConfigCollectionCnv::TrigMonConfigCollectionCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigMonConfigCollection, TrigMonConfigCollection_PERS>(svcloc),
  m_log(new MsgStream(msgSvc(), "TrigMonConfigCollectionCnv")),
  m_impl(new TrigMonConfigCollectionCnv_impl)
{
}

//---------------------------------------------------------------------------------------------
TrigMonConfigCollectionCnv::~TrigMonConfigCollectionCnv()
{
  delete m_log;
  delete m_impl;
}

//---------------------------------------------------------------------------------------------
//create persistent
TrigMonConfigCollection_PERS* TrigMonConfigCollectionCnv::createPersistent(TrigMonConfigCollection* transObj) 
{
  (*m_log) << MSG::DEBUG << "TrigMonConfigCollectionCnv::createPersistent" << endmsg;

  TrigMonConfigCollection_PERS *persObj = m_impl->m_TPConverter_tlp1.createPersistent(transObj, *m_log);
  return persObj;
}

//---------------------------------------------------------------------------------------------
//createTransient
TrigMonConfigCollection* TrigMonConfigCollectionCnv::createTransient() 
{
  (*m_log) << MSG::DEBUG << "TrigMonConfigCollectionCnv::createTransient " << endmsg;
  
  static const pool::Guid tlp1_guid("887A60C5-BD4D-467C-9629-9F0EE818398A");
  static const pool::Guid p1_guid("77FAC318-137E-4563-A681-68356299E397");
  
  TrigMonConfigCollection *trans_obj(0);
  
  if( compareClassGuid(tlp1_guid) ) {
    
    (*m_log) << MSG::DEBUG << "TrigMonConfigCollectionCnv::reading tlp1 persistent object" << endmsg;
    std::unique_ptr< TrigMonConfigCollection_tlp1 >   col_vect( this->poolReadObject< TrigMonConfigCollection_tlp1 >() );
    (*m_log) << MSG::DEBUG << "TrigMonConfigCollectionCnv::reading tlp1 persistent object pointer" <<  col_vect.get() << endmsg;
    trans_obj = m_impl->m_TPConverter_tlp1.createTransient( col_vect.get(), *m_log);
  }
  else if( compareClassGuid(p1_guid) ) {
    
    (*m_log) << MSG::DEBUG << "TrigMonConfigCollectionCnv::reading p1 persistent object" << endmsg;
    std::unique_ptr< TrigMonConfigCollection_p1 >   col_vect( this->poolReadObject< TrigMonConfigCollection_p1 >() );
    (*m_log) << MSG::DEBUG << "TrigMonConfigCollectionCnv::reading p1 persistent object pointer" <<  col_vect.get() << endmsg;
    trans_obj = m_impl->m_TPConverter.createTransient( col_vect.get(), *m_log);
  }
  else {    
    throw std::runtime_error("Unsupported persistent version of TrigMonConfigCollection");
  }

  return trans_obj;
}
