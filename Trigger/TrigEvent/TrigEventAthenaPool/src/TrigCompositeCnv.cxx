/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeCnv.h"
#include "TrigCombinedEventTPCnv/TrigCompositeCnv_p1.h"


struct TrigCompositeCnv_impl
{
  TrigCompositeCnv_p1 m_TPConverter;
};

TrigCompositeCnv::TrigCompositeCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigComposite, TrigComposite_PERS >( svcloc ),
  m_impl (new TrigCompositeCnv_impl)
{}

TrigCompositeCnv::~TrigCompositeCnv()
{
  delete m_impl;
}

TrigComposite_PERS* TrigCompositeCnv::createPersistent(TrigComposite* transCont) 
{
  MsgStream mlog(messageService(), "TrigCompositeConverter" );
  mlog << MSG::DEBUG << "TrigCompositeCnv::createPersistent" << endreq;

  TrigComposite_PERS *persObj = m_impl->m_TPConverter.createPersistent( transCont, mlog );
  
  return persObj;
}


TrigComposite* TrigCompositeCnv::createTransient() 
{
  MsgStream mlog(messageService(), "TrigCompositeConverter" );
  mlog << MSG::DEBUG << "TrigCompositeCnv::createTransient " << endreq;
  
  static pool::Guid p1_guid("53F70884-38EE-4D08-9F03-FA8BED725B63");
  
  TrigComposite       *trans_cont(0);
  
  if( compareClassGuid(p1_guid) ) {
    
    mlog << MSG::DEBUG << "TrigCompositeCnv::reading p1 persistent object" << endreq;
    std::auto_ptr< TrigComposite_p1 >   col_vect( this->poolReadObject< TrigComposite_p1 >() );
    trans_cont = m_impl->m_TPConverter.createTransient( col_vect.get(), mlog );

  } else {

    throw std::runtime_error("Unsupported persistent version of Data container TrigComposite");

  }
  return trans_cont;

}
