/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CMXEtSumsCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by CMXEtSumsCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CMXEtSumsCollectionCnv.h"

//createPersistent 
CMXEtSumsCollection_PERS * CMXEtSumsCollectionCnv::createPersistent( CMXEtSumsCollection *transObj)
{
  
  MsgStream mlog(messageService(), "CMXEtSumsCollectionConverter" );

  //mlog << MSG::DEBUG << "CMXEtSumsCollectionCnv::createPersistent called" << endreq;

  CMXEtSumsCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CMXEtSumsCollection * CMXEtSumsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "CMXEtSumsCollectionConverter" );
  
  //mlog << MSG::DEBUG << "CMXEtSumsCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "8CC9CAAB-5A8B-4581-BA66-C762BA7CB565" );
 
  CMXEtSumsCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< CMXEtSumsCollection_PERS > pers_ref( poolReadObject< CMXEtSumsCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
   
    }else  throw std::runtime_error( "Unsupported persistent version of CMXEtSumsCollection" );
     
  return p_collection;
   
}//end of create transient method

