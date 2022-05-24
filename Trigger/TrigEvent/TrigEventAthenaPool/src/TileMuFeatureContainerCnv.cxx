/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TileMuFeatureContainerCnv.h"

#include "TrigMuonEventTPCnv/TileMuFeatureContainer_p2.h"


//createPersistent
TileMuFeatureContainer_PERS * TileMuFeatureContainerCnv::createPersistent( TileMuFeatureContainer *transObj)
{
  MsgStream mlog(msgSvc(), "TileMuFeatureContainerConverter" );

  mlog << MSG::DEBUG << "TileMuFeatureContainerCnv::createPersistent called" << endmsg;

  TileMuFeatureContainer_PERS * p_cont = m_converter.createPersistent( transObj, mlog );

  return p_cont;

}//end of create persistent method

//createTransient
TileMuFeatureContainer * TileMuFeatureContainerCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TileMuFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "TileMuFeatureContainerCnv::createTransient called" << endmsg;
  
  static const pool::Guid p2_guid( "D7B3C086-B471-47EE-B8EC-E58DC4CD2AD9" );
  static const pool::Guid tlp1_guid( "4684172D-3590-4464-BCA6-477725ADD755" );
  static const pool::Guid p0_guid( "8B428EC4-339C-4517-A047-C0F5F98B820B" );
  
  if( compareClassGuid( p2_guid ) ){
         std::unique_ptr< TileMuFeatureContainer_p2 > col_vect( poolReadObject< TileMuFeatureContainer_p2 >() );
         return m_converter.createTransient( col_vect.get(), mlog ) ;

    }else  if( compareClassGuid( tlp1_guid ) ) {
         std::unique_ptr< TileMuFeatureContainer_tlp1 > col_vect( poolReadObject< TileMuFeatureContainer_tlp1 >() );
         return m_converter1.createTransient( col_vect.get(), mlog );
      
    }else if( compareClassGuid( p0_guid ) ){
      
      return poolReadObject< TileMuFeatureContainer >();
      
    }else  throw std::runtime_error( "Unsupported persistent version of TileMuFeatureContainer" );
  
}//end of create transient method

