/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigMonEventCollectionCnv.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCollectionCnv_p1.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCollectionCnv_tlp1.h"

class TrigMonEventCollectionCnv_impl
{
public:
  TrigMonEventCollectionCnv_p1 m_TPConverter;
  TrigMonEventCollectionCnv_tlp1 m_TPConverter_tlp1;
};

//---------------------------------------------------------------------------------------------
TrigMonEventCollectionCnv::TrigMonEventCollectionCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigMonEventCollection, TrigMonEventCollection_PERS>(svcloc),
  m_log(new MsgStream(messageService(), "TrigMonEventCollectionCnv")),
  m_impl(new TrigMonEventCollectionCnv_impl)
{
}

//---------------------------------------------------------------------------------------------
TrigMonEventCollectionCnv::~TrigMonEventCollectionCnv()
{
  delete m_log;
  delete m_impl;
}

//---------------------------------------------------------------------------------------------
//create persistent
TrigMonEventCollection_PERS* TrigMonEventCollectionCnv::createPersistent(TrigMonEventCollection* transObj) 
{
  (*m_log) << MSG::DEBUG << "TrigMonEventCollectionCnv::createPersistent" << endreq;

  TrigMonEventCollection_PERS *persObj = m_impl->m_TPConverter_tlp1.createPersistent(transObj, *m_log);
  return persObj;
}

//---------------------------------------------------------------------------------------------
//createTransient
TrigMonEventCollection* TrigMonEventCollectionCnv::createTransient() 
{
  (*m_log) << MSG::DEBUG << "TrigMonEventCollectionCnv::createTransient " << endreq;
  
  static pool::Guid tlp1_guid("F224B21A-0CE8-40F1-B88B-027DA832A2A5");
  static pool::Guid p1_guid("9A0D41EB-9A40-42A1-8CDA-C80845FC7271");
  
  TrigMonEventCollection *trans_obj(0);
  
  if( compareClassGuid(tlp1_guid) ) {
    
    (*m_log) << MSG::DEBUG << "TrigMonEventCollectionCnv::reading tlp1 persistent object" << endreq;
    std::auto_ptr< TrigMonEventCollection_tlp1 >   col_vect( this->poolReadObject< TrigMonEventCollection_tlp1 >() );
    (*m_log) << MSG::DEBUG << "TrigMonEventCollectionCnv::reading tlp1 persistent object pointer" <<  col_vect.get() << endreq;
    trans_obj = m_impl->m_TPConverter_tlp1.createTransient(col_vect.get(), *m_log);
  }
  else if( compareClassGuid(p1_guid) ) {
    
    (*m_log) << MSG::DEBUG << "TrigMonEventCollectionCnv::reading p1 persistent object" << endreq;
    std::auto_ptr< TrigMonEventCollection_p1 >   col_vect( this->poolReadObject< TrigMonEventCollection_p1 >() );
    (*m_log) << MSG::DEBUG << "TrigMonEventCollectionCnv::reading p1 persistent object pointer" <<  col_vect.get() << endreq;
    trans_obj = m_impl->m_TPConverter.createTransient(col_vect.get(), *m_log);
  }
  else {
    throw std::runtime_error("Unsupported persistent version of TrigMonEventCollection");
  }

  return trans_obj;
}
