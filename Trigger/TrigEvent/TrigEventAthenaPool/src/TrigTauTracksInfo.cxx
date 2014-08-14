/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauTracksInfoCnv.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfo_tlp1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCnv_tlp1.h"

TrigTauTracksInfoCnv::TrigTauTracksInfoCnv(ISvcLocator* svcloc) :
  T_AthenaPoolCustomCnv<TrigTauTracksInfo, TrigTauTracksInfo_PERS >( svcloc ),
  m_TPConverter (new TrigTauTracksInfoCnv_tlp1)
{}
TrigTauTracksInfoCnv::~TrigTauTracksInfoCnv()
{
  delete m_TPConverter;
}

//create persistent
TrigTauTracksInfo_PERS* TrigTauTracksInfoCnv::createPersistent(TrigTauTracksInfo* transCont) 
{
  MsgStream mlog(messageService(), "TrigTauTracksInfoConverter" );
  mlog << MSG::DEBUG << "TrigTauTracksInfoCnv::createPersistent" << endreq;

  TrigTauTracksInfo_PERS *persObj = m_TPConverter->createPersistent( transCont, mlog );

  return persObj;
}

//create transient
TrigTauTracksInfo* TrigTauTracksInfoCnv::createTransient() 
{
  MsgStream mlog(messageService(), "TrigTauTracksInfoConverter" );
  mlog << MSG::DEBUG << "TrigTauTracksInfoCnv::createTransient " << endreq;
  
  static pool::Guid tlp1_guid("ABF3C91C-A007-440E-8AEE-753FAA453352");
  static pool::Guid p0_guid("630E4944-7EDE-4938-B189-020DBC0436AE");
  
  TrigTauTracksInfo       *trans_cont(0);
  
  if( compareClassGuid(tlp1_guid) ) {
    
    mlog << MSG::DEBUG << "TrigTauTracksInfoCnv::reading tlp1 persistent object" << endreq;
    std::auto_ptr< TrigTauTracksInfo_tlp1 >   col_vect( this->poolReadObject< TrigTauTracksInfo_tlp1 >() );
    trans_cont = m_TPConverter->createTransient( col_vect.get(), mlog );
    
  }
  else if( compareClassGuid(p0_guid) ) {
    
    mlog << MSG::DEBUG << "TrigTauTracksInfoCnv::reading p0 persistent object" << endreq;
    // old version from before TP separation, just return it
    trans_cont = this->poolReadObject<TrigTauTracksInfo>();
    
  }  else {
    
    throw std::runtime_error("Unsupported persistent version of Data container");
    
  }

  return trans_cont;

}
