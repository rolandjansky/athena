/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSpacePointCountsCollectionCnv.h"

#include "TrigInDetEventTPCnv/TrigSpacePointCountsCollection_p4.h"

//createPersistent
TrigSpacePointCountsCollection_PERS * TrigSpacePointCountsCollectionCnv::createPersistent( TrigSpacePointCountsCollection *transObj)
{
  MsgStream mlog(msgSvc(), "TrigSpacePointCountsCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigSpacePointCountsCollectionCnv::createPersistent called" << endmsg;
  
  TrigSpacePointCountsCollection_PERS * p_cont = m_converter.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
TrigSpacePointCountsCollection * TrigSpacePointCountsCollectionCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "TrigSpacePointCountsCollectionConverter" );
  
  mlog << MSG::DEBUG << "TrigSpacePointCountsCollectionCnv::createTransient called" << endmsg;
  
  static const pool::Guid p4_guid( "ACEDF654-09D7-49F6-9054-8E2CEE767367" );
  static const pool::Guid tlp1_guid( "55733D7E-0054-4785-ADA8-3EA70D7477F2" );
  static const pool::Guid p0_guid( "633C9739-C3D1-4F5D-9678-887445DA42B6" );
  
  if( compareClassGuid( p4_guid ) ) {
         std::unique_ptr< TrigSpacePointCountsCollection_p4 > col_vect( poolReadObject< TrigSpacePointCountsCollection_p4 >() );
         //         std::cout << "Reading IMFC p4" << std::endl;
         return m_converter.createTransient( col_vect.get(), mlog ) ;

  }  else if( compareClassGuid( tlp1_guid ) ) {
         std::unique_ptr< TrigSpacePointCountsCollection_tlp1 > col_vect( poolReadObject< TrigSpacePointCountsCollection_tlp1 >() );
         //  std::cout << "Reading IMFC tlp1" << std::endl;
         return m_converter_tlp1.createTransient( col_vect.get(), mlog );

    } else if( compareClassGuid( p0_guid ) ) {
         return poolReadObject< TrigSpacePointCountsCollection >();

    } else throw std::runtime_error( "Unsupported persistent version of TrigSpacePointCountsCollection" );
  
}//end of create transient method

