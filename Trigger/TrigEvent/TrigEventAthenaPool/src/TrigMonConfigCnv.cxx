/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigMonConfigCnv.h"
#include "TrigMonitoringEventTPCnv/TrigMonConfigCnv_p1.h"

//---------------------------------------------------------------------------------------------
TrigMonConfigCnv::TrigMonConfigCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigMonConfig, TrigMonConfig_PERS>(svcloc),
  m_log(new MsgStream(msgSvc(), "TrigMonConfigCnv")),
  m_TPConverter (new TrigMonConfigCnv_p1)
{
}

//---------------------------------------------------------------------------------------------
TrigMonConfigCnv::~TrigMonConfigCnv()
{
  delete m_log;
  delete m_TPConverter;
}

//---------------------------------------------------------------------------------------------
//create persistent
TrigMonConfig_PERS* TrigMonConfigCnv::createPersistent(TrigMonConfig* transObj) 
{
  (*m_log) << MSG::DEBUG << "TrigMonConfigCnv::createPersistent" << endmsg;

  TrigMonConfig_PERS *persObj = m_TPConverter->createPersistent(transObj, *m_log);
  return persObj;
}

//---------------------------------------------------------------------------------------------
//createTransient
TrigMonConfig* TrigMonConfigCnv::createTransient() 
{
  (*m_log) << MSG::DEBUG << "TrigMonConfigCnv::createTransient " << endmsg;
  
  static const pool::Guid p1_guid("E841D3CA-AB5A-4955-850A-B368DE66A987");  
  TrigMonConfig *trans_obj(0);
  
  if(compareClassGuid(p1_guid)) {
    
    (*m_log) << MSG::DEBUG << "TrigMonConfigCnv::reading p1 persistent object" << endmsg;
    std::unique_ptr< TrigMonConfig_p1 >   col_vect( this->poolReadObject< TrigMonConfig_p1 >() );
    trans_obj = m_TPConverter->createTransient( col_vect.get(), *m_log);
  }
  else {    
    throw std::runtime_error("Unsupported persistent version of TrigMonConfig");
  }

  return trans_obj;
}
