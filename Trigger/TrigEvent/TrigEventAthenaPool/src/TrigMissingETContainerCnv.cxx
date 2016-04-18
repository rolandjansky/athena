/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMissingETContainerCnv.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainerCnv_tlp1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p1.h"

#include "TrigMissingEtEventTPCnv/TrigMissingETContainerCnv_p3.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p3.h"

static TrigMissingETContainerCnv_tlp1 converter_tlp1;
static TrigMissingETContainerCnv_p3   TPConverter;   

//createPersistent 
TrigMissingETContainer_PERS * TrigMissingETContainerCnv::createPersistent( TrigMissingETContainer *transObj)
{
  MsgStream mlog(messageService(), "TrigMissingETContainerConverter" );

  mlog << MSG::DEBUG << "TrigMissingETContainerCnv::createPersistent" << endreq;

  TrigMissingETContainer_PERS* persObj = TPConverter.createPersistent( transObj, mlog );
 
  return persObj;
 
}//end of create persistent method
 

//createTransient
TrigMissingETContainer* TrigMissingETContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigMissingETContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigMissingETContainerCnv::createTransient" << endreq;

  static pool::Guid p3_guid( "F5C98A61-4F40-4FE6-A1A9-D5EF00FFFBF0" );
  static pool::Guid tlp1_guid( "23EC84A7-8614-42D6-B82D-B0861D3CE08D" );

  if( compareClassGuid( p3_guid ) ){
         std::auto_ptr< TrigMissingETContainer_p3 > col_vect( poolReadObject< TrigMissingETContainer_p3 >() );
         //         std::cout << "Reading IMFC p3" << std::endl;
         return TPConverter.createTransient( col_vect.get(), mlog ) ;

  } else if( compareClassGuid( tlp1_guid ) ) {
        std::auto_ptr< TrigMissingETContainer_tlp1 > col_vect( poolReadObject< TrigMissingETContainer_tlp1 >() );
         //  std::cout << "Reading IMFC tlp1" << std::endl;
         return converter_tlp1.createTransient( col_vect.get(), mlog );
       
  } else  throw std::runtime_error( "Unsupported persistent version of TrigMissingETContainer" );
  
}//end of create transient method
 
