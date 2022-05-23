/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauClusterDetailsContainerCnv.h"
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainer_p2.h"

//createPersistent 
TrigTauClusterDetailsContainer_PERS * TrigTauClusterDetailsContainerCnv::createPersistent( TrigTauClusterDetailsContainer *transObj)
{
  MsgStream mlog(msgSvc(), "TrigTauClusterDetailsContainerConverter" );

  mlog << MSG::DEBUG << "TrigTauClusterDetailsContainerCnv::createPersistent called" << endmsg;

  TrigTauClusterDetailsContainer_PERS * p_TauClusterCont = m_converter.createPersistent( transObj, mlog );
 
  return p_TauClusterCont;
 
}//end of create persistent method

//createTransient
TrigTauClusterDetailsContainer * TrigTauClusterDetailsContainerCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TrigTauClusterDetailsContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigTauClusterDetailsContainerCnv::createTransient called" << endmsg;

  static const pool::Guid p2_guid( "AAEE63E0-BA47-45AD-AC28-E07EC68812C4" );
  static const pool::Guid tlp1_guid( "D7DA2036-9F38-4060-A5C9-75C72AF104C4" );

  if( compareClassGuid( p2_guid ) ){
         std::unique_ptr< TrigTauClusterDetailsContainer_p2 > col_vect( poolReadObject< TrigTauClusterDetailsContainer_p2 >() );
         //std::cout << "Reading TTCD p2" << std::endl;
         return m_converter.createTransient( col_vect.get(), mlog ) ;

    }else if( compareClassGuid( tlp1_guid ) )     
    {
        std::unique_ptr< TrigTauClusterDetailsContainer_tlp1 > col_vect( poolReadObject< TrigTauClusterDetailsContainer_tlp1 >() );
        //std::cout << "Reading TTCD tlp1" << std::endl;
        return m_converter1.createTransient( col_vect.get(), mlog );
   
    }
  else  throw std::runtime_error( "Unsupported persistent version of TrigTauClusterDetailsContainer" );
  
}//end of create transient method
