/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauTracksInfoCollectionCnv.h"

//createPersistent 
TrigTauTracksInfoCollection_PERS * TrigTauTracksInfoCollectionCnv::createPersistent( TrigTauTracksInfoCollection *transObj)
{
  MsgStream mlog(msgSvc(), "TrigTauTracksInfoCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigTauTracksInfoCollectionCnv::createPersistent called" << endmsg;
  
  TrigTauTracksInfoCollection_PERS * p_cont = m_converter.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
TrigTauTracksInfoCollection * TrigTauTracksInfoCollectionCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TrigTauTracksInfoCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigTauTracksInfoCollectionCnv::createTransient called" << endmsg;
  
  static const pool::Guid p2_guid( "1AF8C4E5-4862-4625-B9B6-D9B53E716B17" );
  static const pool::Guid p1_guid( "8A208FA7-C52F-4CD3-AE20-EF1C99FC92A6" );
  static const pool::Guid p0_guid( "27E95E77-0D99-417D-83C7-7F1B8E6DE511" );
  
  if( compareClassGuid( p2_guid ) ){
         std::unique_ptr< TrigTauTracksInfoCollection_p2 > col_vect( poolReadObject< TrigTauTracksInfoCollection_p2 >() );
         return m_converter.createTransient( col_vect.get(), mlog ) ;
  } else if( compareClassGuid( p1_guid ) ) {
         std::unique_ptr< TrigTauTracksInfoCollection_tlp1 > col_vect( poolReadObject< TrigTauTracksInfoCollection_tlp1 >() );
         return m_converter_tlp1.createTransient( col_vect.get(), mlog );
  } else if( compareClassGuid( p0_guid ) ){
      return poolReadObject< TrigTauTracksInfoCollection >();
  } else  throw std::runtime_error( "Unsupported persistent version of TrigTauTracksInfoCollection" );
  
}//end of create transient method

