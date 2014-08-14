/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigHisto2DContainer_p1.h"
#include "TrigInDetEventTPCnv/TrigHisto2DContainerCnv_tlp1.h"
#include "TrigHisto2DContainerCnv.h"

TrigHisto2DContainerCnv::TrigHisto2DContainerCnv( ISvcLocator *svcloc )
  : TrigHisto2DContainerCnvBase(svcloc),
    m_TPConverter (new TrigHisto2DContainerCnv_tlp1)
{
}
TrigHisto2DContainerCnv::~TrigHisto2DContainerCnv()
{
  delete m_TPConverter;
}
   

//createPersistent 
TrigHisto2DContainer_PERS * TrigHisto2DContainerCnv::createPersistent( TrigHisto2DContainer *transObj)
{
  MsgStream mlog(messageService(), "TrigHisto2DContainerConverter" );

  mlog << MSG::DEBUG << "TrigHisto2DContainerCnv::createPersistent" << endreq;

  TrigHisto2DContainer_PERS* persObj = m_TPConverter->createPersistent( transObj, mlog );
 
  return persObj;
 
}//end of create persistent method
 

//createTransient
TrigHisto2DContainer* TrigHisto2DContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigHisto2DContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigHisto2DContainerCnv::createTransient" << endreq;

  static pool::Guid tlp1_guid( "59C6A169-C537-4DEC-984D-41CE2544F96E" );
  static pool::Guid p1_guid( "C9FE79CB-6875-4E35-A1E3-23DFA6F94DFE" );
  static pool::Guid trans_guid( "E43A29FA-3BC9-4B08-9A94-735E1EC53951" );

  TrigHisto2DContainer *p_container = 0;

  if(compareClassGuid(tlp1_guid)) {
    poolReadObject<TrigHisto2DContainer_PERS>(*m_TPConverter);
    p_container = m_TPConverter->createTransient(mlog);
  }
  else if(compareClassGuid(p1_guid)) {
     std::auto_ptr< TrigHisto2DContainer_p1 > col_vect( poolReadObject< TrigHisto2DContainer_p1 >() );
     TrigHisto2DContainerCnv_p1 converter;
     p_container = converter.createTransient( col_vect.get(), mlog );
  }
  else if(compareClassGuid(trans_guid)) {
    p_container = poolReadObject<TrigHisto2DContainer>();
  }
  else  {
    throw std::runtime_error( "Unsupported persistent version of TrigHisto2DContainer" );
  }
  
  return p_container;
   
}//end of create transient method
 
