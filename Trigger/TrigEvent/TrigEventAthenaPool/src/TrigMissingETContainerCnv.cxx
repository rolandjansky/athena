/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMissingETContainerCnv.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p3.h"

//createPersistent
TrigMissingETContainer_PERS * TrigMissingETContainerCnv::createPersistent( TrigMissingETContainer *transObj)
{
  MsgStream mlog(msgSvc(), "TrigMissingETContainerConverter" );

  mlog << MSG::DEBUG << "TrigMissingETContainerCnv::createPersistent" << endmsg;

  TrigMissingETContainer_PERS* persObj = m_converter.createPersistent( transObj, mlog );
 
  return persObj;
 
}//end of create persistent method
 

//createTransient
TrigMissingETContainer* TrigMissingETContainerCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TrigMissingETContainerConverter" );
  
  mlog << MSG::DEBUG << "TrigMissingETContainerCnv::createTransient" << endmsg;

  static const pool::Guid p3_guid( "F5C98A61-4F40-4FE6-A1A9-D5EF00FFFBF0" );
  static const pool::Guid tlp1_guid( "23EC84A7-8614-42D6-B82D-B0861D3CE08D" );

  if( compareClassGuid( p3_guid ) ){
         std::unique_ptr< TrigMissingETContainer_p3 > col_vect( poolReadObject< TrigMissingETContainer_p3 >() );
         //         std::cout << "Reading IMFC p3" << std::endl;
         return m_converter.createTransient( col_vect.get(), mlog ) ;

  } else if( compareClassGuid( tlp1_guid ) ) {
        std::unique_ptr< TrigMissingETContainer_tlp1 > col_vect( poolReadObject< TrigMissingETContainer_tlp1 >() );
         //  std::cout << "Reading IMFC tlp1" << std::endl;
         return m_converter_tlp1.createTransient( col_vect.get(), mlog );
       
  } else  throw std::runtime_error( "Unsupported persistent version of TrigMissingETContainer" );
  
}//end of create transient method
 
