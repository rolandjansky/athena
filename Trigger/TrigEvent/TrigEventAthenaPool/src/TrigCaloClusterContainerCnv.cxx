/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloClusterContainerCnv.h"

//createPersistent 
TrigCaloClusterContainer_PERS * TrigCaloClusterContainerCnv::createPersistent( TrigCaloClusterContainer *transObj)
{
  MsgStream mlog(msgSvc(), "TrigCaloClusterContainerConverter" );

  mlog << MSG::DEBUG << "TrigCaloClusterContainerCnv::createPersistent called" << endmsg;

  TrigCaloClusterContainer_PERS * p_CaloClusterCont = m_converter.createPersistent( transObj, mlog );
 
  return p_CaloClusterCont;
 
}//end of create persistent method

//createTransient
TrigCaloClusterContainer * TrigCaloClusterContainerCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TrigCaloClusterContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigCaloClusterContainerCnv::createTransient called" << endmsg;

  static const pool::Guid p3_guid( "98A28943-662A-4141-82C3-537447264DA3" );

 if( compareClassGuid( p3_guid ) ){
         std::unique_ptr< TrigCaloClusterContainer_p3 > col_vect( poolReadObject< TrigCaloClusterContainer_p3 >() );
         //         std::cout << "Reading IMFC p3" << std::endl;
         return m_converter.createTransient( col_vect.get(), mlog ) ;
  } else { throw std::runtime_error( "Unsupported persistent version of TrigCaloClusterContainer" ); }
   
}//end of create transient method
 
