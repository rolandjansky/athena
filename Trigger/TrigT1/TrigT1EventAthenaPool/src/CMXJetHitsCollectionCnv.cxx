/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CMXJetHitsCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by CMXJetHitsCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CMXJetHitsCollectionCnv.h"

//createPersistent 
CMXJetHitsCollection_PERS * CMXJetHitsCollectionCnv::createPersistent( CMXJetHitsCollection *transObj)
{
  
  MsgStream mlog(messageService(), "CMXJetHitsCollectionConverter" );

  //mlog << MSG::DEBUG << "CMXJetHitsCollectionCnv::createPersistent called" << endreq;

  CMXJetHitsCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CMXJetHitsCollection * CMXJetHitsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "CMXJetHitsCollectionConverter" );
  
  //mlog << MSG::DEBUG << "CMXJetHitsCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "29388465-59FE-452C-9462-45C3022F5F2B" );
 
  CMXJetHitsCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< CMXJetHitsCollection_PERS > pers_ref( poolReadObject< CMXJetHitsCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
      
    }else  throw std::runtime_error( "Unsupported persistent version of CMXJetHitsCollection" );
     
  return p_collection;
   
}//end of create transient method

