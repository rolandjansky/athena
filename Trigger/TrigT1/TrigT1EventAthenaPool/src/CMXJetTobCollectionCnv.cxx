/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CMXJetTobCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by CMXJetTobCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CMXJetTobCollectionCnv.h"

//createPersistent 
CMXJetTobCollection_PERS * CMXJetTobCollectionCnv::createPersistent( CMXJetTobCollection *transObj)
{
  
  MsgStream mlog(messageService(), "CMXJetTobCollectionConverter" );

  //mlog << MSG::DEBUG << "CMXJetTobCollectionCnv::createPersistent called" << endreq;

  CMXJetTobCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CMXJetTobCollection * CMXJetTobCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "CMXJetTobCollectionConverter" );
  
  //mlog << MSG::DEBUG << "CMXJetTobCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "3FD32EB7-6573-4BCD-A510-6A5549E495A4" );
 
  CMXJetTobCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< CMXJetTobCollection_PERS > pers_ref( poolReadObject< CMXJetTobCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
      
    }else  throw std::runtime_error( "Unsupported persistent version of CMXJetTobCollection" );
     
  return p_collection;
   
}//end of create transient method

