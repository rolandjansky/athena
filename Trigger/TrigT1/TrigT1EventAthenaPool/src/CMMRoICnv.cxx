/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CMMRoICnv.cxx
 * @brief Generated implementation file which includes header files needed by CMMRoICnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CMMRoICnv.h"

using namespace LVL1;

//createPersistent 
CMMRoI_PERS * CMMRoICnv::createPersistent( CMMRoI *transObj)
{
  
  MsgStream mlog(messageService(), "CMMRoIConverter" );

  //mlog << MSG::DEBUG << "CMMRoICnv::createPersistent called" << endreq;

  CMMRoI_PERS * persObj = m_TPConverter_p1.createPersistent( transObj, mlog );
 
  return persObj;
 
}//end of create persistent method
 

//createTransient
CMMRoI * CMMRoICnv::createTransient()
{
  MsgStream mlog(messageService(), "CMMRoIConverter" );
  
  //mlog << MSG::DEBUG << "CMMRoICnv::createTransient called" << endreq;

  static pool::Guid p1_guid( "AF2D0BF8-8B28-4BFD-92B1-67F4708039C3" );
 
  
  if ( compareClassGuid(p1_guid) ) {
     // using auto_ptr ensures deletion of the persistent object
     std::auto_ptr< CMMRoI_p1 > pers_ref( poolReadObject< CMMRoI_p1 >() );
     return m_TPConverter_p1.createTransient( pers_ref.get(), mlog );
  }
  throw std::runtime_error("Unsupported persistent version of CMMRoI");

  return 0;
   
}//end of create transient method

