/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEMClusterContainerCnv.h"


//createPersistent 
TrigEMClusterContainer_PERS * TrigEMClusterContainerCnv::createPersistent( TrigEMClusterContainer *transObj)
{
  MsgStream mlog(msgSvc(), "TrigEMClusterContainerConverter" );

  mlog << MSG::DEBUG << "TrigEMClusterContainerCnv::createPersistent called" << endmsg;

  TrigEMClusterContainer_PERS * p_EMClusterCont = m_converter.createPersistent( transObj, mlog );
 
  return p_EMClusterCont;
 
}//end of create persistent method

//createTransient
TrigEMClusterContainer * TrigEMClusterContainerCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TrigEMClusterContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigEMClusterContainerCnv::createTransient called" << endmsg;

  static const pool::Guid p3_guid( "0BF627E6-52A0-4F10-9FFD-A513DF2DBC31" );
  static const pool::Guid p4_guid( "7B430CA8-5D16-4E26-B0A4-461F983610EB" );
  static const pool::Guid tlp1_guid( "CAA8145E-F382-4727-A114-26E137D4B27E" );

 if( compareClassGuid( p4_guid ) ){
         std::unique_ptr< TrigEMClusterContainer_p4 > col_vect( poolReadObject< TrigEMClusterContainer_p4 >() );
         //         std::cout << "Reading IMFC p4" << std::endl;
         return m_converter.createTransient( col_vect.get(), mlog ) ;
 } else if( compareClassGuid( p3_guid ) ){
         std::unique_ptr< TrigEMClusterContainer_p3 > col_vect( poolReadObject< TrigEMClusterContainer_p3 >() );
         return m_converter_p3.createTransient( col_vect.get(), mlog ) ;
  } else if( compareClassGuid( tlp1_guid ) ) {
        std::unique_ptr< TrigEMClusterContainer_tlp1 > col_vect( poolReadObject< TrigEMClusterContainer_tlp1 >() );
        return m_converter_tlp1.createTransient( col_vect.get(), mlog );
  } else { throw std::runtime_error( "Unsupported persistent version of TrigEMClusterContainer" ); }
   
}//end of create transient method
 
