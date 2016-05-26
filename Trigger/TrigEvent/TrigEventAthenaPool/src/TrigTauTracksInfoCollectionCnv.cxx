/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauTracksInfoCollectionCnv.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollectionCnv_tlp1.h"
#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollectionCnv_p2.h"

static TrigTauTracksInfoCollectionCnv_tlp1 TPConverter_tlp1;
static TrigTauTracksInfoCollectionCnv_p2   TPConverter;

//createPersistent 
TrigTauTracksInfoCollection_PERS * TrigTauTracksInfoCollectionCnv::createPersistent( TrigTauTracksInfoCollection *transObj)
{
  MsgStream mlog(messageService(), "TrigTauTracksInfoCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigTauTracksInfoCollectionCnv::createPersistent called" << endreq;
  
  TrigTauTracksInfoCollection_PERS * p_cont = TPConverter.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
TrigTauTracksInfoCollection * TrigTauTracksInfoCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "TrigTauTracksInfoCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigTauTracksInfoCollectionCnv::createTransient called" << endreq;
  
  static pool::Guid p2_guid( "1AF8C4E5-4862-4625-B9B6-D9B53E716B17" );
  static pool::Guid p1_guid( "8A208FA7-C52F-4CD3-AE20-EF1C99FC92A6" );
  static pool::Guid p0_guid( "27E95E77-0D99-417D-83C7-7F1B8E6DE511" );
  
  if( compareClassGuid( p2_guid ) ){
         std::auto_ptr< TrigTauTracksInfoCollection_p2 > col_vect( poolReadObject< TrigTauTracksInfoCollection_p2 >() );
         return TPConverter.createTransient( col_vect.get(), mlog ) ;
  } else if( compareClassGuid( p1_guid ) ) {
         std::auto_ptr< TrigTauTracksInfoCollection_tlp1 > col_vect( poolReadObject< TrigTauTracksInfoCollection_tlp1 >() );
         return TPConverter_tlp1.createTransient( col_vect.get(), mlog );
  } else if( compareClassGuid( p0_guid ) ){
      return poolReadObject< TrigTauTracksInfoCollection >();
  } else  throw std::runtime_error( "Unsupported persistent version of TrigTauTracksInfoCollection" );
  
}//end of create transient method

