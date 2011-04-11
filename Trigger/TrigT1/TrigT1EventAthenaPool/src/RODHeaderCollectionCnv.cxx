/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file RODHeaderCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by RODHeaderCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "RODHeaderCollectionCnv.h"

//createPersistent 
RODHeaderCollection_PERS * RODHeaderCollectionCnv::createPersistent( RODHeaderCollection *transObj)
{
  
  MsgStream mlog(messageService(), "RODHeaderCollectionConverter" );

  //mlog << MSG::DEBUG << "RODHeaderCollectionCnv::createPersistent called" << endreq;

  RODHeaderCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
RODHeaderCollection * RODHeaderCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "RODHeaderCollectionConverter" );
  
  //mlog << MSG::DEBUG << "RODHeaderCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "8226F1B6-374D-424D-B059-E0A1B18A1DA7" );
 
  RODHeaderCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< RODHeaderCollection_PERS > pers_ref( poolReadObject< RODHeaderCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
   
    }else  throw std::runtime_error( "Unsupported persistent version of RODHeaderCollection" );
     
  return p_collection;
   
}//end of create transient method

