/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CMXCPTobCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by CMXCPTobCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CMXCPTobCollectionCnv.h"

//createPersistent 
CMXCPTobCollection_PERS * CMXCPTobCollectionCnv::createPersistent( CMXCPTobCollection *transObj)
{
  
  MsgStream mlog(messageService(), "CMXCPTobCollectionConverter" );

  //mlog << MSG::DEBUG << "CMXCPTobCollectionCnv::createPersistent called" << endreq;

  CMXCPTobCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CMXCPTobCollection * CMXCPTobCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "CMXCPTobCollectionConverter" );
  
  //mlog << MSG::DEBUG << "CMXCPTobCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "0E42AA28-7C10-4934-9E54-97C38C249544" );
 
  CMXCPTobCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< CMXCPTobCollection_PERS > pers_ref( poolReadObject< CMXCPTobCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
   
    }else  throw std::runtime_error( "Unsupported persistent version of CMXCPTobCollection" );
     
  return p_collection;
   
}//end of create transient method

