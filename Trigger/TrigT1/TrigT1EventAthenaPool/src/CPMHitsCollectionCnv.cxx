/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CPMHitsCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by CPMHitsCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CPMHitsCollectionCnv.h"

//createPersistent 
CPMHitsCollection_PERS * CPMHitsCollectionCnv::createPersistent( CPMHitsCollection *transObj)
{
  
  MsgStream mlog(messageService(), "CPMHitsCollectionConverter" );

  //mlog << MSG::DEBUG << "CPMHitsCollectionCnv::createPersistent called" << endreq;

  CPMHitsCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CPMHitsCollection * CPMHitsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "CPMHitsCollectionConverter" );
  
  //mlog << MSG::DEBUG << "CPMHitsCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "4A309D12-C11D-4666-8253-72522BF7948E" );
 
  CPMHitsCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< CPMHitsCollection_PERS > pers_ref( poolReadObject< CPMHitsCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
      
    }else  throw std::runtime_error( "Unsupported persistent version of CPMHitsCollection" );
     
  return p_collection;
   
}//end of create transient method

