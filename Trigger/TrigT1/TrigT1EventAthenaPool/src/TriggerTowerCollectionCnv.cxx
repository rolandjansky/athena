/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TriggerTowerCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by TriggerTowerCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "TriggerTowerCollectionCnv.h"

//createPersistent 
TriggerTowerCollection_PERS * TriggerTowerCollectionCnv::createPersistent( TriggerTowerCollection *transObj)
{
  
  MsgStream mlog(messageService(), "TriggerTowerCollectionConverter" );

  //mlog << MSG::DEBUG << "TriggerTowerCollectionCnv::createPersistent called" << endreq;

  TriggerTowerCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
TriggerTowerCollection * TriggerTowerCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "TriggerTowerCollectionConverter" );
  
  //mlog << MSG::DEBUG << "TriggerTowerCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "CDF34B04-7594-4A52-8EE0-FE85CF2C2A18" );
  static pool::Guid p0_guid( "1B6DA81A-2F27-4B50-ACF6-D3FC19B40ADB" );
 
  TriggerTowerCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< TriggerTowerCollection_PERS > pers_ref( poolReadObject< TriggerTowerCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
      
    }else if( compareClassGuid( p0_guid ) ){

      p_collection = poolReadObject< TriggerTowerCollection >();
    
    }else  throw std::runtime_error( "Unsupported persistent version of TriggerTowerCollection" );
     
  return p_collection;
   
}//end of create transient method

