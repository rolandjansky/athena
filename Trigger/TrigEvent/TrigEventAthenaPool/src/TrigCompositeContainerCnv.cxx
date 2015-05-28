/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeContainerCnv.h"
#include "TrigCombinedEventTPCnv/TrigCompositeContainerCnv_p1.h"

static TrigCompositeContainerCnv_p1   TPConverter;

//createPersistent 
TrigCompositeContainer_PERS * TrigCompositeContainerCnv::createPersistent( TrigCompositeContainer *transObj)
{
  MsgStream mlog(messageService(), "TrigCompositeContainerConverter" );

  mlog << MSG::DEBUG << "TrigCompositeContainerCnv::createPersistent called" << endreq;

  TrigCompositeContainer_PERS * p_CompositeCont = TPConverter.createPersistent( transObj, mlog );
 
  return p_CompositeCont;
 
}//end of create persistent method

//createTransient
TrigCompositeContainer * TrigCompositeContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigCompositeContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigCompositeContainerCnv::createTransient called" << endreq;

  static pool::Guid p1_guid( "6D16EA6B-968C-41F0-B5E8-9CC6D5BD7F9A" );

  
 if( compareClassGuid( p1_guid ) ){
         std::auto_ptr< TrigCompositeContainer_p1 > col_vect( poolReadObject< TrigCompositeContainer_p1 >() );
         //         std::cout << "Reading IMFC p1" << std::endl; 
         return TPConverter.createTransient( col_vect.get(), mlog ) ;
  } else { throw std::runtime_error( "Unsupported persistent version of TrigCompositeContainer" ); }
   
}//end of create transient method
 
