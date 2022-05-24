/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTrackMuFeatureContainerCnv.h"

#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainer_p3.h"


//createPersistent 
TileTrackMuFeatureContainer_PERS * TileTrackMuFeatureContainerCnv::createPersistent( TileTrackMuFeatureContainer *transObj)
{
  MsgStream mlog(msgSvc(), "TileTrackMuFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "TileTrackMuFeatureContainerCnv::createPersistent called" << endmsg;

  TileTrackMuFeatureContainer_PERS * p_cont = m_converter.createPersistent( transObj, mlog );

  return p_cont;
  
}//end of create persistent method


//createTransient
TileTrackMuFeatureContainer * TileTrackMuFeatureContainerCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TileTrackMuFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "TileTrackMuFeatureContainerCnv::createTransient called" << endmsg;

  static const pool::Guid p3_guid( "25018D00-1D18-4B4C-9C07-37993260EADB");
  static const pool::Guid tlp1_guid( "983ED5FE-D0A2-43AE-90A9-268C8B61E8B3");
  static const pool::Guid p0_guid( "1AFABC18-EB97-412D-B27C-C744ABA6E1DC");

  if( compareClassGuid( p3_guid ) ){
         std::unique_ptr< TileTrackMuFeatureContainer_p3 > col_vect( poolReadObject< TileTrackMuFeatureContainer_p3 >() );
         return m_converter.createTransient( col_vect.get(), mlog ) ;
  
    }else if( compareClassGuid( tlp1_guid ) ) {
         std::unique_ptr< TileTrackMuFeatureContainer_tlp1 > col_vect( poolReadObject< TileTrackMuFeatureContainer_tlp1 >() );
         return m_converter1.createTransient( col_vect.get(), mlog );

    }else if( compareClassGuid( p0_guid ) ){
      
         return poolReadObject< TileTrackMuFeatureContainer >();
      
    }else  throw std::runtime_error( "Unsupported persistent version of TileTrackMuFeatureContainer" );
  
}//end of create transient method

