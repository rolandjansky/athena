/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigHisto1DContainer_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto1DContainerCnv_tlp1.h"
#include "TrigHisto1DContainerCnv.h"

TrigHisto1DContainerCnv::TrigHisto1DContainerCnv( ISvcLocator *svcloc )
  : TrigHisto1DContainerCnvBase(svcloc),
    m_TPConverter (new TrigHisto1DContainerCnv_tlp1)
{
}

TrigHisto1DContainerCnv::~TrigHisto1DContainerCnv()
{
  delete m_TPConverter;
}
   

//createPersistent 
TrigHisto1DContainer_PERS * TrigHisto1DContainerCnv::createPersistent( TrigHisto1DContainer *transObj)
{
  MsgStream mlog(messageService(), "TrigHisto1DContainerConverter" );

  mlog << MSG::DEBUG << "TrigHisto1DContainerCnv::createPersistent" << endreq;

  TrigHisto1DContainer_PERS* persObj = m_TPConverter->createPersistent( transObj, mlog );
 
  return persObj;
 
}//end of create persistent method
 

//createTransient
TrigHisto1DContainer* TrigHisto1DContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigHisto1DContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigHisto1DContainerCnv::createTransient" << endreq;

  static pool::Guid tlp1_guid( "CDC70E22-85DF-45B4-B69E-FC7FD70987A9" );
  static pool::Guid p1_guid( "B6240954-1842-45F5-AB5B-50934C633B1E" );
  static pool::Guid trans_guid( "B5752F0C-EF7A-4330-8413-1CCF0FC58AC8" );

  TrigHisto1DContainer *p_container = 0;

  if(compareClassGuid(tlp1_guid)) {
    poolReadObject<TrigHisto1DContainer_PERS>(*m_TPConverter);
    p_container = m_TPConverter->createTransient(mlog);
  }
  else if(compareClassGuid(p1_guid)) {
     std::auto_ptr< TrigHisto1DContainer_p1 > col_vect( poolReadObject< TrigHisto1DContainer_p1 >() );
     TrigHisto1DContainerCnv_p1 converter;
     p_container = converter.createTransient( col_vect.get(), mlog );
  }
  else if(compareClassGuid(trans_guid)) {
    p_container = poolReadObject<TrigHisto1DContainer>();
  }
  else  {
    throw std::runtime_error( "Unsupported persistent version of TrigHisto1DContainer" );
  }
  
  return p_container;
   
}//end of create transient method
 
