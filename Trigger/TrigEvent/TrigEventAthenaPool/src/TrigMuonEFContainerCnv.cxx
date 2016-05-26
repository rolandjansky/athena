/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEFContainerCnv.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainerCnv_tlp1.h"

#include "TrigMuonEventTPCnv/TrigMuonEFContainer_p2.h"
#include "TrigMuonEventTPCnv/TrigMuonEFContainerCnv_p2.h"


static TrigMuonEFContainerCnv_tlp1 TPConverter_tlp1;
static TrigMuonEFContainerCnv_p2   TPConverter;

//createPersistent 
TrigMuonEFContainer_PERS * TrigMuonEFContainerCnv::createPersistent( TrigMuonEFContainer *transObj)
{
  MsgStream mlog(messageService(), "TrigMuonEFContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigMuonEFContainerCnv::createPersistent called" << endreq;

  TrigMuonEFContainer_PERS * p_muEFcont = TPConverter.createPersistent( transObj, mlog ); 

  return p_muEFcont;

}//end of create persistent method


//createTransient
TrigMuonEFContainer * TrigMuonEFContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigMuonEFContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigMuonEFContainerCnv::createTransient called" << endreq;
  
  static pool::Guid p2_guid( "3BB5C107-2975-4F9E-AACC-9FD948C3D2A8" );
  static pool::Guid p1_guid( "567E65A0-23E8-469F-94F7-A03F6E9C1C2E" );
  static pool::Guid p0_guid( "BB866230-C9D8-437A-A11B-A0CC08ACD97B" );
  
  if( compareClassGuid( p2_guid ) ){
         std::auto_ptr< TrigMuonEFContainer_p2 > col_vect( poolReadObject< TrigMuonEFContainer_p2 >() );
         //         std::cout << "Reading IMFC p2" << std::endl;
         return TPConverter.createTransient( col_vect.get(), mlog ) ;

  } else if( compareClassGuid( p1_guid ) ) {
         std::auto_ptr< TrigMuonEFContainer_tlp1 > col_vect( poolReadObject< TrigMuonEFContainer_tlp1 >() );
         //  std::cout << "Reading IMFC tlp1" << std::endl;
         return TPConverter_tlp1.createTransient( col_vect.get(), mlog );
      
  } else if( compareClassGuid( p0_guid ) ){
      
         return poolReadObject< TrigMuonEFContainer >();
      
    } else  throw std::runtime_error( "Unsupported persistent version of TrigMuonEFContainer" );
  
}//end of create transient method

