/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CMMEtSumsCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by CMMEtSumsCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CMMEtSumsCollectionCnv.h"

//createPersistent 
CMMEtSumsCollection_PERS * CMMEtSumsCollectionCnv::createPersistent( CMMEtSumsCollection *transObj)
{
  
  MsgStream mlog(messageService(), "CMMEtSumsCollectionConverter" );

  //mlog << MSG::DEBUG << "CMMEtSumsCollectionCnv::createPersistent called" << endreq;

  CMMEtSumsCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CMMEtSumsCollection * CMMEtSumsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "CMMEtSumsCollectionConverter" );
  
  //mlog << MSG::DEBUG << "CMMEtSumsCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "DAB02AE0-736D-4C24-AC21-E6F12B3A4FE3" );
 
  CMMEtSumsCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< CMMEtSumsCollection_PERS > pers_ref( poolReadObject< CMMEtSumsCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
   
    }else  throw std::runtime_error( "Unsupported persistent version of CMMEtSumsCollection" );
     
  return p_collection;
   
}//end of create transient method

