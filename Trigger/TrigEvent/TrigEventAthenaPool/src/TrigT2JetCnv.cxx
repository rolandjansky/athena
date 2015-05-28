/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2JetCnv.h"
#include "TrigCaloEventTPCnv/TrigT2JetCnv_tlp1.h"
 
TrigT2JetCnv::TrigT2JetCnv( ISvcLocator *svcloc ):
  TrigT2JetCnvBase(svcloc),
  m_msgSvc( msgSvc() ),
  m_log( m_msgSvc, "TrigT2JetCnv" ),
  m_TPConverter (new TrigT2JetCnv_tlp1)
{}

TrigT2JetCnv::~TrigT2JetCnv()
{
  delete m_TPConverter;
}


//createPersistent
TrigT2Jet_PERS * TrigT2JetCnv::createPersistent( TrigT2Jet *transObj)
{
  MsgStream mlog(messageService(), "TrigT2JetConverter" );

  mlog << MSG::DEBUG << "TrigT2JetCnv::createPersistent called" << endreq;

  TrigT2Jet_PERS * p_T2Jet = m_TPConverter->createPersistent( transObj, m_log );
 
  return p_T2Jet;
 
}//end of create persistent method
 


//createTransient
TrigT2Jet * TrigT2JetCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigT2JetConverter" );
  
  mlog << MSG::DEBUG << "TrigT2JetCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "EAF10B4D-AD26-4030-AF0C-1060A39D663D" );
  static pool::Guid p0_guid( "242473A8-A320-49F1-A680-136EA26C1FAF" );
 
  TrigT2Jet *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
      poolReadObject< TrigT2Jet_PERS >(*m_TPConverter);
      p_collection = m_TPConverter->createTransient( m_log );
   
    }else if( compareClassGuid( p0_guid ) ){

      p_collection = poolReadObject< TrigT2Jet >();
    
    }else  throw std::runtime_error( "Unsupported persistent version of TrigT2Jet" );
     
  return p_collection;
   
}//end of create transient method
 
