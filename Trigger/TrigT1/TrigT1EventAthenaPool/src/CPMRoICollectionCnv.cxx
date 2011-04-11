/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CPMRoICollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by CPMRoICollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CPMRoICollectionCnv.h"

//createPersistent 
CPMRoICollection_PERS * CPMRoICollectionCnv::createPersistent( CPMRoICollection *transObj)
{
  
  MsgStream mlog(messageService(), "CPMRoICollectionConverter" );

  //mlog << MSG::DEBUG << "CPMRoICollectionCnv::createPersistent called" << endreq;

  CPMRoICollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CPMRoICollection * CPMRoICollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "CPMRoICollectionConverter" );
  
  //mlog << MSG::DEBUG << "CPMRoICollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "A5867077-621F-4B04-8777-56846085E550" );
 
  CPMRoICollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
           // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< CPMRoICollection_PERS > pers_ref( poolReadObject< CPMRoICollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );

    }else  throw std::runtime_error( "Unsupported persistent version of CPMRoICollection" );
     
  return p_collection;
   
}//end of create transient method

