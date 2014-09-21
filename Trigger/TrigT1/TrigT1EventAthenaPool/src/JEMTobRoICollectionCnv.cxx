/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JEMTobRoICollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by JEMTobRoICollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "JEMTobRoICollectionCnv.h"

//createPersistent 
JEMTobRoICollection_PERS * JEMTobRoICollectionCnv::createPersistent( JEMTobRoICollection *transObj)
{
  
  MsgStream mlog(messageService(), "JEMTobRoICollectionConverter" );

  //mlog << MSG::DEBUG << "JEMTobRoICollectionCnv::createPersistent called" << endreq;

  JEMTobRoICollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
JEMTobRoICollection * JEMTobRoICollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "JEMTobRoICollectionConverter" );
  
  //mlog << MSG::DEBUG << "JEMTobRoICollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "49790040-16E0-4CAB-96A2-31DC010FFB06" );
 
  JEMTobRoICollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< JEMTobRoICollection_PERS > pers_ref( poolReadObject< JEMTobRoICollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
   
    }else  throw std::runtime_error( "Unsupported persistent version of JEMTobRoICollection" );
     
  return p_collection;
   
}//end of create transient method

