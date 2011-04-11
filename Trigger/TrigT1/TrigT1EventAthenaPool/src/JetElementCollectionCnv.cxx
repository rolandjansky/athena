/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file JetElementCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by JetElementCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

#include "JetElementCollectionCnv.h"

//createPersistent 
JetElementCollection_PERS * JetElementCollectionCnv::createPersistent( JetElementCollection *transObj)
{
  
  MsgStream mlog(messageService(), "JetElementCollectionConverter" );

  //mlog << MSG::DEBUG << "JetElementCollectionCnv::createPersistent called" << endreq;

  JetElementCollection_PERS * p_TTColl = m_TPConverter_tlp1.createPersistent( transObj, mlog );
 
  return p_TTColl;
 
}//end of create persistent method
 

//createTransient
JetElementCollection * JetElementCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "JetElementCollectionConverter" );
  
  //mlog << MSG::DEBUG << "JetElementCollectionCnv::createTransient called" << endreq;

  static pool::Guid tlp1_guid( "132ABB53-1F89-4E0E-83A3-382338B278F3" );
  static pool::Guid p0_guid( "48B92167-4892-4694-9E9E-E201F1E1FFFE" );
 
  JetElementCollection *p_collection = 0;
  if( compareClassGuid( tlp1_guid ) )     
    {
     // using auto_ptr ensures deletion of the persistent object
      std::auto_ptr< JetElementCollection_PERS > pers_ref( poolReadObject< JetElementCollection_PERS >() );
      p_collection = m_TPConverter_tlp1.createTransient( pers_ref.get(), mlog );
   
    }else if( compareClassGuid( p0_guid ) ){

      p_collection = poolReadObject< JetElementCollection >();
    
    }else throw std::runtime_error( "Unsupported persistent version of JetElementCollection" );

    return p_collection;
   
}//end of create transient method

