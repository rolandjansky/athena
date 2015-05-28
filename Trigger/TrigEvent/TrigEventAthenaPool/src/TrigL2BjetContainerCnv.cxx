/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2BjetContainerCnv.h"
#include "TrigParticleTPCnv/TrigL2BjetContainerCnv_tlp1.h"
#include "TrigParticleTPCnv/TrigL2BjetContainerCnv_tlp2.h"

#include "TrigParticleTPCnv/TrigL2BjetContainer_p3.h"
#include "TrigParticleTPCnv/TrigL2BjetContainerCnv_p3.h"

static TrigL2BjetContainerCnv_tlp1   TLPconverter1;
static TrigL2BjetContainerCnv_tlp2   TLPconverter2;
static TrigL2BjetContainerCnv_p3     TPconverter;

//* createPersistent *// 
TrigL2BjetContainer_PERS * TrigL2BjetContainerCnv::createPersistent( TrigL2BjetContainer *transObj) {

  MsgStream mlog(messageService(), "TrigL2BjetContainerConverter" );

  mlog << MSG::DEBUG << "TrigL2BjetContainerCnv::createPersistent called" << endreq;

  TrigL2BjetContainer_PERS * p_L2BjetCont = TPconverter.createPersistent( transObj, mlog );
 
  return p_L2BjetCont;
 
}

//* createTransient *//
TrigL2BjetContainer * TrigL2BjetContainerCnv::createTransient() {

  MsgStream mlog(messageService(), "TrigL2BjetContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigL2BjetContainerCnv::createTransient called" << endreq;

  static pool::Guid p3_guid( "65534D07-EECA-46B1-A6AA-A4DA4DFEAAEF" );
  static pool::Guid tlp2_guid( "67162A5D-CEC3-4D9B-AF43-1858D6F1558E" );
  static pool::Guid tlp1_guid( "716A64FC-6EAF-4851-A0C8-8CE73C43C051" );
  static pool::Guid p0_guid( "B4842D15-7BFB-476E-8C68-F2F38E588380" );
  static pool::Guid p0_guid2( "F6ACED03-42F3-4192-A4E2-47FA9A9B9D49" );
  
  if( compareClassGuid( p3_guid ) ){
         std::auto_ptr< TrigL2BjetContainer_p3 > col_vect( poolReadObject< TrigL2BjetContainer_p3 >() );
         //         std::cout << "Reading IMFC p3" << std::endl;
         return TPconverter.createTransient( col_vect.get(), mlog ) ;

  } else if ( compareClassGuid( tlp2_guid ) ) {
    
         std::auto_ptr< TrigL2BjetContainer_tlp2 > col_vect( poolReadObject< TrigL2BjetContainer_tlp2 >() );
         //  std::cout << "Reading IMFC tlp2" << std::endl;
         return TLPconverter2.createTransient( col_vect.get(), mlog );

  } else if ( compareClassGuid( tlp1_guid ) ) {
    
         std::auto_ptr< TrigL2BjetContainer_tlp1 > col_vect( poolReadObject< TrigL2BjetContainer_tlp1 >() );
         //  std::cout << "Reading IMFC tlp1" << std::endl;
         return TLPconverter1.createTransient( col_vect.get(), mlog );
    
  } else if( compareClassGuid( p0_guid ) || compareClassGuid( p0_guid2 ) ) {
    
    return poolReadObject< TrigL2BjetContainer >();
    
  } else  throw std::runtime_error( "Unsupported persistent version of TrigL2BjetContainer" );
  
}
 
