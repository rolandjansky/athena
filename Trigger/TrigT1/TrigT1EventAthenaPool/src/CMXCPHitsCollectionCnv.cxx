/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CMXCPHitsCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by CMXCPHitsCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CMXCPHitsCollectionCnv.h"

//createPersistent 
CMXCPHitsCollection_PERS * CMXCPHitsCollectionCnv::createPersistent( CMXCPHitsCollection *transObj)
{
  
  MsgStream mlog(messageService(), "CMXCPHitsCollectionConverter" );

  //mlog << MSG::DEBUG << "CMXCPHitsCollectionCnv::createPersistent called" << endreq;

  CMXCPHitsCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CMXCPHitsCollection * CMXCPHitsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "CMXCPHitsCollectionConverter" );
  
  //mlog << MSG::DEBUG << "CMXCPHitsCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "1A1F6DEF-0233-453C-8614-E7B82EFCEDC6" );
 
  CMXCPHitsCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< CMXCPHitsCollection_PERS > pers_ref( poolReadObject< CMXCPHitsCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
   
    }else  throw std::runtime_error( "Unsupported persistent version of CMXCPHitsCollection" );
     
  return p_collection;
   
}//end of create transient method

