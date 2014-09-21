/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CPMTobRoICollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by CPMTobRoICollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CPMTobRoICollectionCnv.h"

//createPersistent 
CPMTobRoICollection_PERS * CPMTobRoICollectionCnv::createPersistent( CPMTobRoICollection *transObj)
{
  
  MsgStream mlog(messageService(), "CPMTobRoICollectionConverter" );

  //mlog << MSG::DEBUG << "CPMTobRoICollectionCnv::createPersistent called" << endreq;

  CPMTobRoICollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CPMTobRoICollection * CPMTobRoICollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "CPMTobRoICollectionConverter" );
  
  //mlog << MSG::DEBUG << "CPMTobRoICollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "E575088B-8CD0-471C-A278-EA6E508A171E" );
 
  CPMTobRoICollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
           // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< CPMTobRoICollection_PERS > pers_ref( poolReadObject< CPMTobRoICollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );

    }else  throw std::runtime_error( "Unsupported persistent version of CPMTobRoICollection" );
     
  return p_collection;
   
}//end of create transient method

