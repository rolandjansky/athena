/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CMXRoICnv.cxx
 * @brief Generated implementation file which includes header files needed by CMXRoICnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CMXRoICnv.h"


using namespace LVL1;


//createPersistent 
CMXRoI_PERS * CMXRoICnv::createPersistent( CMXRoI *transObj)
{
  
  MsgStream mlog(msgSvc(), "CMXRoIConverter" );

  //mlog << MSG::DEBUG << "CMXRoICnv::createPersistent called" << endmsg;

  CMXRoI_PERS * persObj = m_TPConverter_p1.createPersistent( transObj, mlog );
 
  return persObj;
 
}//end of create persistent method
 

//createTransient
CMXRoI * CMXRoICnv::createTransient()
{
  MsgStream mlog(msgSvc(), "CMXRoIConverter" );
  
  //mlog << MSG::DEBUG << "CMXRoICnv::createTransient called" << endmsg;

  static const pool::Guid p1_guid( "6EE27E92-E8DE-4F07-810F-025A1450E3BE" );
 
  
  if ( compareClassGuid(p1_guid) ) {
     // using unique_ptr ensures deletion of the persistent object
     std::unique_ptr< CMXRoI_p1 > pers_ref( poolReadObject< CMXRoI_p1 >() );
     return m_TPConverter_p1.createTransient( pers_ref.get(), mlog );
  }
  throw std::runtime_error("Unsupported persistent version of CMXRoI");

  return 0;
   
}//end of create transient method

