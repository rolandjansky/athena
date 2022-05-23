/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CPMTowerCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by CPMTowerCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "CPMTowerCollectionCnv.h"

//createPersistent 
CPMTowerCollection_PERS * CPMTowerCollectionCnv::createPersistent( CPMTowerCollection *transObj)
{
  
  MsgStream mlog(msgSvc(), "CPMTowerCollectionConverter" );

  //mlog << MSG::DEBUG << "CPMTowerCollectionCnv::createPersistent called" << endmsg;

  CPMTowerCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
CPMTowerCollection * CPMTowerCollectionCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "CPMTowerCollectionConverter" );
  
  //mlog << MSG::DEBUG << "CPMTowerCollectionCnv::createTransient called" << endmsg;

  static const pool::Guid tlp1_guid( "AF950A9C-D929-4E3D-8B95-53D0B36B78D8" );
 
  CPMTowerCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr< CPMTowerCollection_PERS > pers_ref( poolReadObject< CPMTowerCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
   
    }else  throw std::runtime_error( "Unsupported persistent version of CPMTowerCollection" );
     
  return p_collection;
   
}//end of create transient method

