/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauClusterDetailsCnv.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetails_tlp1.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_tlp1.h"

TrigTauClusterDetailsCnv::TrigTauClusterDetailsCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigTauClusterDetails, TrigTauClusterDetails_PERS >( svcloc ),
  m_TPConverter (new TrigTauClusterDetailsCnv_tlp1)
{}
TrigTauClusterDetailsCnv::~TrigTauClusterDetailsCnv()
{
  delete m_TPConverter;
}


//create persistent
TrigTauClusterDetails_PERS* TrigTauClusterDetailsCnv::createPersistent(TrigTauClusterDetails* transCont) 
{
  MsgStream mlog(messageService(), "TrigTauClusterDetailsConverter" );
  mlog << MSG::DEBUG << "TrigTauClusterDetailsCnv::createPersistent" << endreq;

  TrigTauClusterDetails_PERS *persObj = m_TPConverter->createPersistent( transCont, mlog );

  return persObj;

}

//create transient
TrigTauClusterDetails* TrigTauClusterDetailsCnv::createTransient() 
{
  MsgStream mlog(messageService(), "TrigTauClusterDetailsConverter" );
  mlog << MSG::DEBUG << "TrigTauClusterDetailsCnv::createTransient " << endreq;

  static pool::Guid tlp1_guid("4F82A265-04F7-4BE9-9DB1-42C00A574778");
  static pool::Guid p0_guid("1C8021CB-F4FB-473A-B379-9BEF3FC7FEB9");

  TrigTauClusterDetails       *trans_cont(0);

  if( compareClassGuid(tlp1_guid) ) {
    
    mlog << MSG::DEBUG << "TrigTauClusterDetailsCnv::reading tlp1 persistent object" << endreq;
    std::auto_ptr< TrigTauClusterDetails_tlp1 >   col_vect( this->poolReadObject< TrigTauClusterDetails_tlp1 >() );
    trans_cont = m_TPConverter->createTransient( col_vect.get(), mlog );

  }
  else if( compareClassGuid(p0_guid) ) {

    mlog << MSG::DEBUG << "TrigTauClusterDetailsCnv::reading p0 persistent object" << endreq;
    // old version from before TP separation, just return it
    trans_cont = this->poolReadObject<TrigTauClusterDetails>();

    }  else {

      throw std::runtime_error("Unsupported persistent version of Data container");
    }
  
  return trans_cont;
}
