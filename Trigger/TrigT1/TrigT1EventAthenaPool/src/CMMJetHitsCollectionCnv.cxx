/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CMMJetHitsCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by CMMJetHitsCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CMMJetHitsCollectionCnv.h"

//createPersistent 
CMMJetHitsCollection_PERS * CMMJetHitsCollectionCnv::createPersistent( CMMJetHitsCollection *transObj)
{
  
  MsgStream mlog(msgSvc(), "CMMJetHitsCollectionConverter" );

  //mlog << MSG::DEBUG << "CMMJetHitsCollectionCnv::createPersistent called" << endmsg;

  CMMJetHitsCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CMMJetHitsCollection * CMMJetHitsCollectionCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "CMMJetHitsCollectionConverter" );
  
  //mlog << MSG::DEBUG << "CMMJetHitsCollectionCnv::createTransient called" << endmsg;

  static const pool::Guid tlp1_guid( "972519B8-F2B3-4205-A0B1-06678460918F" );
 
  CMMJetHitsCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr< CMMJetHitsCollection_PERS > pers_ref( poolReadObject< CMMJetHitsCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
      
    }else  throw std::runtime_error( "Unsupported persistent version of CMMJetHitsCollection" );
     
  return p_collection;
   
}//end of create transient method

