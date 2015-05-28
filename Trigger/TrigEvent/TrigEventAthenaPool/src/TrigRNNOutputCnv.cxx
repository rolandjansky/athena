/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigRNNOutputCnv.h"
#include "TrigCaloEventTPCnv/TrigRNNOutputCnv_tlp1.h"

struct TrigRNNOutputCnv_impl
{
  TrigRNNOutputCnv_tlp1 m_TPConverter;
};

TrigRNNOutputCnv::TrigRNNOutputCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigRNNOutput, TrigRNNOutput_PERS >( svcloc ),
  m_impl (new TrigRNNOutputCnv_impl)
{}

TrigRNNOutputCnv::~TrigRNNOutputCnv()
{
  delete m_impl;
}

TrigRNNOutput_PERS* TrigRNNOutputCnv::createPersistent(TrigRNNOutput* transCont) 
{
  MsgStream mlog(messageService(), "TrigRNNOutputConverter" );
  mlog << MSG::DEBUG << "TrigRNNOutputCnv::createPersistent" << endreq;

  TrigRNNOutput_PERS *persObj = m_impl->m_TPConverter.createPersistent( transCont, mlog );
  
  return persObj;
}


TrigRNNOutput* TrigRNNOutputCnv::createTransient() 
{
  MsgStream mlog(messageService(), "TrigRNNOutputConverter" );
  mlog << MSG::DEBUG << "TrigRNNOutputCnv::createTransient " << endreq;
  
  static pool::Guid tlp1_guid("BA106EC3-F776-DE11-BCB3-000423DD5A1A");
  
  TrigRNNOutput       *trans_cont(0);
  
  if (compareClassGuid(tlp1_guid)) {
    mlog << MSG::DEBUG << "TrigRNNOutputCnv::reading tlp1 persistent object" << endreq;
    std::auto_ptr< TrigRNNOutput_tlp1 >   col_vect( this->poolReadObject< TrigRNNOutput_tlp1 >() );
    trans_cont = m_impl->m_TPConverter.createTransient( col_vect.get(), mlog );
  }  else {
    throw std::runtime_error("Unsupported persistent version of Data container");
  }
  return trans_cont;

}
