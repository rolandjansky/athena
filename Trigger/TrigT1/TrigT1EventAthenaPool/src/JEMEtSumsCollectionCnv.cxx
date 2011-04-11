/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JEMEtSumsCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by JEMEtSumsCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "JEMEtSumsCollectionCnv.h"

//createPersistent 
JEMEtSumsCollection_PERS * JEMEtSumsCollectionCnv::createPersistent( JEMEtSumsCollection *transObj)
{
  
  MsgStream mlog(messageService(), "JEMEtSumsCollectionConverter" );

  //mlog << MSG::DEBUG << "JEMEtSumsCollectionCnv::createPersistent called" << endreq;

  JEMEtSumsCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
JEMEtSumsCollection * JEMEtSumsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "JEMEtSumsCollectionConverter" );
  
  //mlog << MSG::DEBUG << "JEMEtSumsCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "FFEF6FF1-31E5-492A-9A44-4AD5B79DB22B" );
 
  JEMEtSumsCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< JEMEtSumsCollection_PERS > pers_ref( poolReadObject< JEMEtSumsCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
   
    }else  throw std::runtime_error( "Unsupported persistent version of JEMEtSumsCollection" );
     
  return p_collection;
   
}//end of create transient method

