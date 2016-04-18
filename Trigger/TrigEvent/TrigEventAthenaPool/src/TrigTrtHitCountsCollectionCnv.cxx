/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_p1.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollectionCnv_tlp1.h"
#include "TrigTrtHitCountsCollectionCnv.h"

#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollection_p2.h"
#include "TrigInDetEventTPCnv/TrigTrtHitCountsCollectionCnv_p2.h"

static TrigTrtHitCountsCollectionCnv_tlp1 TPConverter_tlp1;
static TrigTrtHitCountsCollectionCnv_p2   TPConverter;

//createPersistent 
TrigTrtHitCountsCollection_PERS * TrigTrtHitCountsCollectionCnv::createPersistent( TrigTrtHitCountsCollection *transObj)
{
  MsgStream mlog(messageService(), "TrigTrtHitCountsCollectionConverter" );

  mlog << MSG::DEBUG << "TrigTrtHitCountsCollectionCnv::createPersistent" << endreq;

  TrigTrtHitCountsCollection_PERS* persObj = TPConverter.createPersistent( transObj, mlog );
 
  return persObj;
 
}//end of create persistent method

//createTransient
TrigTrtHitCountsCollection* TrigTrtHitCountsCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigTrtHitCountsCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigTrtHitCountsCollectionCnv::createTransient" << endreq;

  static pool::Guid p2_guid( "47CBB4D9-381C-423E-A560-A7B5C325A5DD" );
  static pool::Guid tlp1_guid( "A0763CCA-553C-4365-8091-04CA2036FD97" );
  static pool::Guid p1_guid( "0CC00AC1-FB95-4E69-8C6E-29B6BB713AAE" );
  static pool::Guid trans_guid( "7631C2C2-612F-4245-8C8B-D40F59222E1E" );

  if( compareClassGuid( p2_guid ) ){
         std::auto_ptr< TrigTrtHitCountsCollection_p2 > col_vect( poolReadObject< TrigTrtHitCountsCollection_p2 >() );
         //         std::cout << "Reading IMFC p2" << std::endl;
         return TPConverter.createTransient( col_vect.get(), mlog ) ;

  } else if(compareClassGuid(tlp1_guid)) {

         std::auto_ptr< TrigTrtHitCountsCollection_tlp1 > col_vect( poolReadObject< TrigTrtHitCountsCollection_tlp1 >() );
         //  std::cout << "Reading IMFC tlp1" << std::endl;
         return TPConverter_tlp1.createTransient( col_vect.get(), mlog );

  } else if(compareClassGuid(p1_guid)) {
         std::auto_ptr< TrigTrtHitCountsCollection_p1 > col_vect( poolReadObject< TrigTrtHitCountsCollection_p1 >() );
         TrigTrtHitCountsCollectionCnv_p1 converter;
         return converter.createTransient( col_vect.get(), mlog );

  } else if(compareClassGuid(trans_guid)) {
         return poolReadObject<TrigTrtHitCountsCollection>();

  } else  {

    throw std::runtime_error( "Unsupported persistent version of TrigTrtHitCountsCollection" );

  }
  
}//end of create transient method
 
