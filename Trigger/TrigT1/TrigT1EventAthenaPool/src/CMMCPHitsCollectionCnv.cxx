/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CMMCPHitsCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by CMMCPHitsCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CMMCPHitsCollectionCnv.h"

//createPersistent 
CMMCPHitsCollection_PERS * CMMCPHitsCollectionCnv::createPersistent( CMMCPHitsCollection *transObj)
{
  
  MsgStream mlog(messageService(), "CMMCPHitsCollectionConverter" );

  //mlog << MSG::DEBUG << "CMMCPHitsCollectionCnv::createPersistent called" << endreq;

  CMMCPHitsCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CMMCPHitsCollection * CMMCPHitsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "CMMCPHitsCollectionConverter" );
  
  //mlog << MSG::DEBUG << "CMMCPHitsCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "8C6E0191-FAE9-4C2E-BD0A-E6A5518145E2" );
 
  CMMCPHitsCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< CMMCPHitsCollection_PERS > pers_ref( poolReadObject< CMMCPHitsCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
   
    }else  throw std::runtime_error( "Unsupported persistent version of CMMCPHitsCollection" );
     
  return p_collection;
   
}//end of create transient method

