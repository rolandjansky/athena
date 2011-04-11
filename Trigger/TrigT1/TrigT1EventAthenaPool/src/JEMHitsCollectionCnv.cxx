/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JEMHitsCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by JEMHitsCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "JEMHitsCollectionCnv.h"

//createPersistent 
JEMHitsCollection_PERS * JEMHitsCollectionCnv::createPersistent( JEMHitsCollection *transObj)
{
  
  MsgStream mlog(messageService(), "JEMHitsCollectionConverter" );

  //mlog << MSG::DEBUG << "JEMHitsCollectionCnv::createPersistent called" << endreq;

  JEMHitsCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
JEMHitsCollection * JEMHitsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "JEMHitsCollectionConverter" );
  
  //mlog << MSG::DEBUG << "JEMHitsCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "A9D89195-219E-4F33-9B19-4684639CCA5B" );
 
  JEMHitsCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< JEMHitsCollection_PERS > pers_ref( poolReadObject< JEMHitsCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
      
    }else  throw std::runtime_error( "Unsupported persistent version of JEMHitsCollection" );
     
  return p_collection;
   
}//end of create transient method

