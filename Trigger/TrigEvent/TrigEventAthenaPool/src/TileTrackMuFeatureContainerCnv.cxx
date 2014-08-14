/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileTrackMuFeatureContainerCnv.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainerCnv_tlp1.h"

#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/TileTrackMuFeatureContainerCnv_p3.h"

static TileTrackMuFeatureContainerCnv_tlp1   TLPconverter1;
static TileTrackMuFeatureContainerCnv_p3     TPconverter;

//createPersistent 
TileTrackMuFeatureContainer_PERS * TileTrackMuFeatureContainerCnv::createPersistent( TileTrackMuFeatureContainer *transObj)
{
  MsgStream mlog(messageService(), "TileTrackMuFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "TileTrackMuFeatureContainerCnv::createPersistent called" << endreq;

  TileTrackMuFeatureContainer_PERS * p_cont = TPconverter.createPersistent( transObj, mlog );

  return p_cont;
  
}//end of create persistent method


//createTransient
TileTrackMuFeatureContainer * TileTrackMuFeatureContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "TileTrackMuFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "TileTrackMuFeatureContainerCnv::createTransient called" << endreq;

  static pool::Guid p3_guid( "25018D00-1D18-4B4C-9C07-37993260EADB");  
  static pool::Guid tlp1_guid( "983ED5FE-D0A2-43AE-90A9-268C8B61E8B3");
  static pool::Guid p0_guid( "1AFABC18-EB97-412D-B27C-C744ABA6E1DC");

  if( compareClassGuid( p3_guid ) ){
         std::auto_ptr< TileTrackMuFeatureContainer_p3 > col_vect( poolReadObject< TileTrackMuFeatureContainer_p3 >() );
         return TPconverter.createTransient( col_vect.get(), mlog ) ;
  
    }else if( compareClassGuid( tlp1_guid ) ) {
         std::auto_ptr< TileTrackMuFeatureContainer_tlp1 > col_vect( poolReadObject< TileTrackMuFeatureContainer_tlp1 >() );
         return TLPconverter1.createTransient( col_vect.get(), mlog );

    }else if( compareClassGuid( p0_guid ) ){
      
         return poolReadObject< TileTrackMuFeatureContainer >();
      
    }else  throw std::runtime_error( "Unsupported persistent version of TileTrackMuFeatureContainer" );
  
}//end of create transient method

