/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileMuFeatureContainerCnv.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainerCnv_tlp1.h"

#include "TrigMuonEventTPCnv/TileMuFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/TileMuFeatureContainerCnv_p2.h"

static TileMuFeatureContainerCnv_tlp1   TLPconverter1;
static TileMuFeatureContainerCnv_p2     TPconverter;

//createPersistent
TileMuFeatureContainer_PERS * TileMuFeatureContainerCnv::createPersistent( TileMuFeatureContainer *transObj)
{
  MsgStream mlog(messageService(), "TileMuFeatureContainerConverter" );

  mlog << MSG::DEBUG << "TileMuFeatureContainerCnv::createPersistent called" << endreq;

  TileMuFeatureContainer_PERS * p_cont = TPconverter.createPersistent( transObj, mlog );

  return p_cont;

}//end of create persistent method

//createTransient
TileMuFeatureContainer * TileMuFeatureContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "TileMuFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "TileMuFeatureContainerCnv::createTransient called" << endreq;
  
  static pool::Guid p2_guid( "D7B3C086-B471-47EE-B8EC-E58DC4CD2AD9" );
  static pool::Guid tlp1_guid( "4684172D-3590-4464-BCA6-477725ADD755" );
  static pool::Guid p0_guid( "8B428EC4-339C-4517-A047-C0F5F98B820B" );
  
  if( compareClassGuid( p2_guid ) ){
         std::auto_ptr< TileMuFeatureContainer_p2 > col_vect( poolReadObject< TileMuFeatureContainer_p2 >() );
         return TPconverter.createTransient( col_vect.get(), mlog ) ;

    }else  if( compareClassGuid( tlp1_guid ) ) {
         std::auto_ptr< TileMuFeatureContainer_tlp1 > col_vect( poolReadObject< TileMuFeatureContainer_tlp1 >() );
         return TLPconverter1.createTransient( col_vect.get(), mlog );
      
    }else if( compareClassGuid( p0_guid ) ){
      
      return poolReadObject< TileMuFeatureContainer >();
      
    }else  throw std::runtime_error( "Unsupported persistent version of TileMuFeatureContainer" );
  
}//end of create transient method

