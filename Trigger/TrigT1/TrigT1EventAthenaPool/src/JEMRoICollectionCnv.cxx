/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JEMRoICollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by JEMRoICollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "JEMRoICollectionCnv.h"

//createPersistent 
JEMRoICollection_PERS * JEMRoICollectionCnv::createPersistent( JEMRoICollection *transObj)
{
  
  MsgStream mlog(msgSvc(), "JEMRoICollectionConverter" );

  //mlog << MSG::DEBUG << "JEMRoICollectionCnv::createPersistent called" << endmsg;

  JEMRoICollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
JEMRoICollection * JEMRoICollectionCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "JEMRoICollectionConverter" );
  
  //mlog << MSG::DEBUG << "JEMRoICollectionCnv::createTransient called" << endmsg;

  static const pool::Guid tlp1_guid( "B82ECDA7-30FE-4F02-9BF3-EC507BA8CB04" );
 
  JEMRoICollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr< JEMRoICollection_PERS > pers_ref( poolReadObject< JEMRoICollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
   
    }else  throw std::runtime_error( "Unsupported persistent version of JEMRoICollection" );
     
  return p_collection;
   
}//end of create transient method

