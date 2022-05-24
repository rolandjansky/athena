/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  
  MsgStream mlog(msgSvc(), "CMMEtSumsCollectionConverter" );

  //mlog << MSG::DEBUG << "CMMEtSumsCollectionCnv::createPersistent called" << endmsg;

  CMMEtSumsCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CMMEtSumsCollection * CMMEtSumsCollectionCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "CMMEtSumsCollectionConverter" );
  
  //mlog << MSG::DEBUG << "CMMEtSumsCollectionCnv::createTransient called" << endmsg;

  static const pool::Guid tlp1_guid( "DAB02AE0-736D-4C24-AC21-E6F12B3A4FE3" );
 
  CMMEtSumsCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr< CMMEtSumsCollection_PERS > pers_ref( poolReadObject< CMMEtSumsCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
   
    }else  throw std::runtime_error( "Unsupported persistent version of CMMEtSumsCollection" );
     
  return p_collection;
   
}//end of create transient method

