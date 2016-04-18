/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEMClusterContainerCnv.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainerCnv_tlp1.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainerCnv_p4.h"
#include "TrigCaloEventTPCnv/TrigEMClusterContainerCnv_p3.h"

static TrigEMClusterContainerCnv_tlp1 TPConverter_tlp1;
static TrigEMClusterContainerCnv_p4   TPConverter;
static TrigEMClusterContainerCnv_p3   TPConverter_p3;

//createPersistent 
TrigEMClusterContainer_PERS * TrigEMClusterContainerCnv::createPersistent( TrigEMClusterContainer *transObj)
{
  MsgStream mlog(messageService(), "TrigEMClusterContainerConverter" );

  mlog << MSG::DEBUG << "TrigEMClusterContainerCnv::createPersistent called" << endreq;

  TrigEMClusterContainer_PERS * p_EMClusterCont = TPConverter.createPersistent( transObj, mlog );
 
  return p_EMClusterCont;
 
}//end of create persistent method

//createTransient
TrigEMClusterContainer * TrigEMClusterContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigEMClusterContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigEMClusterContainerCnv::createTransient called" << endreq;

  static pool::Guid p3_guid( "0BF627E6-52A0-4F10-9FFD-A513DF2DBC31" );
  static pool::Guid p4_guid( "7B430CA8-5D16-4E26-B0A4-461F983610EB" );
  static pool::Guid tlp1_guid( "CAA8145E-F382-4727-A114-26E137D4B27E" );

 if( compareClassGuid( p4_guid ) ){
         std::auto_ptr< TrigEMClusterContainer_p4 > col_vect( poolReadObject< TrigEMClusterContainer_p4 >() );
         //         std::cout << "Reading IMFC p4" << std::endl;
         return TPConverter.createTransient( col_vect.get(), mlog ) ;
 } else if( compareClassGuid( p3_guid ) ){
         std::auto_ptr< TrigEMClusterContainer_p3 > col_vect( poolReadObject< TrigEMClusterContainer_p3 >() );
         return TPConverter_p3.createTransient( col_vect.get(), mlog ) ;
  } else if( compareClassGuid( tlp1_guid ) ) {
        std::auto_ptr< TrigEMClusterContainer_tlp1 > col_vect( poolReadObject< TrigEMClusterContainer_tlp1 >() );
        return TPConverter_tlp1.createTransient( col_vect.get(), mlog );
  } else { throw std::runtime_error( "Unsupported persistent version of TrigEMClusterContainer" ); }
   
}//end of create transient method
 
