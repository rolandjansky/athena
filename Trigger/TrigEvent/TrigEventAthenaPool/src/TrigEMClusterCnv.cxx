/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEMClusterCnv.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_tlp2.h"

struct TrigEMClusterCnv_impl
{
  TrigEMClusterCnv_tlp1 m_TPConverter;
  TrigEMClusterCnv_tlp2 m_TPConverter2;
};

TrigEMClusterCnv::TrigEMClusterCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigEMCluster, TrigEMCluster_PERS >( svcloc ),
  m_impl (new TrigEMClusterCnv_impl)
{}

TrigEMClusterCnv::~TrigEMClusterCnv()
{
  delete m_impl;
}

TrigEMCluster_PERS* TrigEMClusterCnv::createPersistent(TrigEMCluster* transCont) 
{
  MsgStream mlog(messageService(), "TrigEMClusterConverter" );
  mlog << MSG::DEBUG << "TrigEMClusterCnv::createPersistent" << endreq;

  TrigEMCluster_PERS *persObj = m_impl->m_TPConverter2.createPersistent( transCont, mlog );
  
  return persObj;
}


TrigEMCluster* TrigEMClusterCnv::createTransient() 
{
  MsgStream mlog(messageService(), "TrigEMClusterConverter" );
  mlog << MSG::DEBUG << "TrigEMClusterCnv::createTransient " << endreq;
  
  static pool::Guid tlp2_guid("61D02BB3-0F97-4809-AC93-72B2DC2DF624");
  static pool::Guid tlp1_guid("EF5124F2-7B9C-4CBE-8D7F-1AD368A8A125");
  static pool::Guid p0_guid("E60986AE-938E-4256-A100-CD99158ADE9E");
  
  TrigEMCluster       *trans_cont(0);
  
  if( compareClassGuid(tlp2_guid) ) {
    
    mlog << MSG::DEBUG << "TrigEMClusterCnv::reading tlp2 persistent object" << endreq;
    std::auto_ptr< TrigEMCluster_tlp2 >   col_vect( this->poolReadObject< TrigEMCluster_tlp2 >() );
    trans_cont = m_impl->m_TPConverter2.createTransient( col_vect.get(), mlog );

  } else if( compareClassGuid(tlp1_guid) ) {
    
    mlog << MSG::DEBUG << "TrigEMClusterCnv::reading tlp1 persistent object" << endreq;
    std::auto_ptr< TrigEMCluster_tlp1 >   col_vect( this->poolReadObject< TrigEMCluster_tlp1 >() );
    trans_cont = m_impl->m_TPConverter.createTransient( col_vect.get(), mlog );

  }
  else if( compareClassGuid(p0_guid) ) {

    mlog << MSG::DEBUG << "TrigEMClusterCnv::reading p0 persistent object" << endreq;
    // old version from before TP separation, just return it
    trans_cont = this->poolReadObject<TrigEMCluster>();

  }  else {

    throw std::runtime_error("Unsupported persistent version of Data container");

  }
  return trans_cont;

}
