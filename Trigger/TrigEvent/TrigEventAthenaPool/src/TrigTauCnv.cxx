/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauCnv.h"
#include "TrigParticleTPCnv/TrigTau_tlp1.h"
#include "TrigParticleTPCnv/TrigTauCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigTau_tlp2.h"
#include "TrigParticleTPCnv/TrigTauCnv_tlp2.h"

class TrigTauCnv_impl
{
public:
  TrigTauCnv_tlp1 m_TPConverter_p1;
  TrigTauCnv_tlp2 m_TPConverter_p2;
};

TrigTauCnv::TrigTauCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigTau, TrigTau_PERS >( svcloc ),
  m_impl (new TrigTauCnv_impl)
{
}
TrigTauCnv::~TrigTauCnv()
{
  delete m_impl;
}

//create persistent
TrigTau_PERS* TrigTauCnv::createPersistent(TrigTau* transCont) 
{
  MsgStream mlog(msgSvc(), "TrigTauConverter" );
  mlog << MSG::DEBUG << "TrigTauCnv::createPersistent" << endmsg;

  TrigTau_PERS *persObj = m_impl->m_TPConverter_p2.createPersistent( transCont, mlog );

  return persObj;
}

//create transient
TrigTau* TrigTauCnv::createTransient() 
{
  MsgStream mlog(msgSvc(), "TrigTauConverter" );
  mlog << MSG::DEBUG << "TrigTauCnv::createTransient " << endmsg;
  
  static const pool::Guid tlp1_guid("82AE3333-5398-4590-A51A-616832332D9B");
  static const pool::Guid p0_guid("F95B5B76-13D3-4EB4-94BB-1383B8571ADD");
  static const pool::Guid tlp2_guid("B2619D46-FB89-4961-8A57-6A980A2EFB33");
  static const pool::Guid p2_guid("9CA7EFF2-DB12-4E06-A425-01B1D8367BE3");
  
  TrigTau       *trans_cont(0);
  
  if( compareClassGuid(tlp1_guid) ) {
    
    mlog << MSG::DEBUG << "TrigTauCnv::reading tlp1 persistent object" << endmsg;
    std::unique_ptr< TrigTau_tlp1 >   col_vect( this->poolReadObject< TrigTau_tlp1 >() );
    trans_cont = m_impl->m_TPConverter_p1.createTransient( col_vect.get(), mlog );
    
  }
  else if( compareClassGuid(p0_guid) ) {
    
    mlog << MSG::DEBUG << "TrigTauCnv::reading p0 persistent object" << endmsg;
    // old version from before TP separation, just return it
    trans_cont = this->poolReadObject<TrigTau>();
    
  }else if( compareClassGuid(tlp2_guid) ) {
    
    mlog << MSG::DEBUG << "TrigTauCnv::reading tlp2 persistent object" << endmsg;
    std::unique_ptr< TrigTau_tlp2 >   col_vect( this->poolReadObject< TrigTau_tlp2 >() );
    trans_cont = m_impl->m_TPConverter_p2.createTransient( col_vect.get(), mlog );
    
  }
  else if( compareClassGuid(p2_guid) ) {
    
    mlog << MSG::DEBUG << "TrigTauCnv::reading p2 persistent object" << endmsg;
    // old version from before TP separation, just return it
    trans_cont = this->poolReadObject<TrigTau>();
    
  }  else {
    
    throw std::runtime_error("Unsupported persistent version of Data container");
    
  }

  return trans_cont;

}
