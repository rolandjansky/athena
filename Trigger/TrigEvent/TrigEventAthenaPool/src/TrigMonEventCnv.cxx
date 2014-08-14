/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigMonEventCnv.h"
#include "TrigMonitoringEventTPCnv/TrigMonEventCnv_p1.h"

//---------------------------------------------------------------------------------------------
TrigMonEventCnv::TrigMonEventCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigMonEvent, TrigMonEvent_PERS>(svcloc),
  m_log(new MsgStream(messageService(), "TrigMonEventCnv")),
  m_TPConverter (new TrigMonEventCnv_p1)
{
}

//---------------------------------------------------------------------------------------------
TrigMonEventCnv::~TrigMonEventCnv()
{
  delete m_log;
  delete m_TPConverter;
}

//---------------------------------------------------------------------------------------------
//create persistent
TrigMonEvent_PERS* TrigMonEventCnv::createPersistent(TrigMonEvent* transObj) 
{
  (*m_log) << MSG::DEBUG << "TrigMonEventCnv::createPersistent" << endreq;

  TrigMonEvent_PERS *persObj = m_TPConverter->createPersistent(transObj, *m_log);
  return persObj;
}

//---------------------------------------------------------------------------------------------
//createTransient
TrigMonEvent* TrigMonEventCnv::createTransient() 
{
  (*m_log) << MSG::DEBUG << "TrigMonEventCnv::createTransient " << endreq;
  
  static pool::Guid p1_guid("AECD5837-E8DA-4FF3-8601-44B4C17355ED");  
  TrigMonEvent *trans_obj(0);
  
  if( compareClassGuid(p1_guid) ) {
    
    (*m_log) << MSG::DEBUG << "TrigMonEventCnv::reading p1 persistent object" << endreq;
    std::auto_ptr< TrigMonEvent_p1 >   col_vect( this->poolReadObject< TrigMonEvent_p1 >());
    trans_obj = m_TPConverter->createTransient( col_vect.get(), *m_log);
  } 
  else {    
    throw std::runtime_error("Unsupported persistent version of TrigMonEvent");
  }

  return trans_obj;
}
