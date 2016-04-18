/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauClusterCnv.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauCluster_tlp2.h"
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_tlp2.h"

class TrigTauClusterCnv_impl
{
public:
  TrigTauClusterCnv_tlp1 m_TPConverter1;
  TrigTauClusterCnv_tlp2 m_TPConverter2;
};

TrigTauClusterCnv::TrigTauClusterCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigTauCluster, TrigTauCluster_PERS >( svcloc ),
  m_impl (new TrigTauClusterCnv_impl)
{
}
TrigTauClusterCnv::~TrigTauClusterCnv()
{
  delete m_impl;
}


//create persistent
TrigTauCluster_PERS* TrigTauClusterCnv::createPersistent(TrigTauCluster* transCont) 
{
  MsgStream mlog(messageService(), "TrigTauClusterConverter" );
  mlog << MSG::DEBUG << "TrigTauClusterCnv::createPersistent" << endreq;

  TrigTauCluster_PERS *persObj = m_impl->m_TPConverter2.createPersistent( transCont, mlog );

  return persObj;

}

//create transient
TrigTauCluster* TrigTauClusterCnv::createTransient() 
{
  MsgStream mlog(messageService(), "TrigTauClusterConverter" );
  mlog << MSG::DEBUG << "TrigTauClusterCnv::createTransient " << endreq;

  static pool::Guid tlp2_guid("CEDD0095-1134-4CEC-91CC-61028B214F6C");
  static pool::Guid tlp1_guid("60C9B591-CE4B-473C-9CB9-8C4F99271D76");
  static pool::Guid p0_guid("9B3B6AE8-9CE6-44AB-8B31-8B39FED52607");

  TrigTauCluster       *trans_cont(0);

  if( compareClassGuid(tlp2_guid) ) {
    
    mlog << MSG::DEBUG << "TrigTauClusterCnv::reading tlp2 persistent object" << endreq;
    std::auto_ptr< TrigTauCluster_tlp2 >   col_vect( this->poolReadObject< TrigTauCluster_tlp2 >() );
    trans_cont = m_impl->m_TPConverter2.createTransient( col_vect.get(), mlog );

  }
  else if( compareClassGuid(tlp1_guid) ) {
    
    mlog << MSG::DEBUG << "TrigTauClusterCnv::reading tlp1 persistent object" << endreq;
    std::auto_ptr< TrigTauCluster_tlp1 >   col_vect( this->poolReadObject< TrigTauCluster_tlp1 >() );
    trans_cont = m_impl->m_TPConverter1.createTransient( col_vect.get(), mlog );

  }
  else if( compareClassGuid(p0_guid) ) {

    mlog << MSG::DEBUG << "TrigTauClusterCnv::reading p0 persistent object" << endreq;
    // old version from before TP separation, just return it
    trans_cont = this->poolReadObject<TrigTauCluster>();

    }  else {

      throw std::runtime_error("Unsupported persistent version of Data container");
    }
  
  return trans_cont;
}
