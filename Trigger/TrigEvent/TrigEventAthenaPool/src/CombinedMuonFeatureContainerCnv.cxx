/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CombinedMuonFeatureContainerCnv.h"

#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p4.h"

//createPersistent
CombinedMuonFeatureContainer_PERS * CombinedMuonFeatureContainerCnv::createPersistent( CombinedMuonFeatureContainer *transObj)
{
  MsgStream mlog(msgSvc(), "CombinedMuonFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "CombinedMuonFeatureContainerCnv::createPersistent called" << endmsg;
  
  CombinedMuonFeatureContainer_PERS * p_cont = m_converter.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
CombinedMuonFeatureContainer * CombinedMuonFeatureContainerCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "CombinedMuonFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "CombinedMuonFeatureContainerCnv::createTransient called" << endmsg;
  
  static const pool::Guid p4_guid( "A06B9B51-FFA9-4AC1-9079-8AF49C4A9B87" );
  static const pool::Guid p3_guid( "02262E5F-7645-4919-9CD9-66D88796DCFA" );
  static const pool::Guid p2_guid( "E9D9F99E-D64F-4114-90EA-236FAF9063BB" );
  static const pool::Guid tlp1_guid( "9C558B18-1B1D-4186-AF26-A5F3EACCBE1B" );
  static const pool::Guid p0_guid( "574BFA1C-ADB8-40DB-B538-0CA20E20CAAD" );
  
  //CombinedMuonFeatureContainer *p_collection = 0;
  if( compareClassGuid( p4_guid ) ){
         std::unique_ptr< CombinedMuonFeatureContainer_p4 > col_vect( poolReadObject< CombinedMuonFeatureContainer_p4 >() );
	 //         std::cout << "Reading CMFC p2" << std::endl;
         return m_converter.createTransient( col_vect.get(), mlog ) ;

  }else if( compareClassGuid( p3_guid ) ){
         std::unique_ptr< CombinedMuonFeatureContainer_p3 > col_vect( poolReadObject< CombinedMuonFeatureContainer_p3 >() );
	 //         std::cout << "Reading CMFC p2" << std::endl;
         return m_converter3.createTransient( col_vect.get(), mlog ) ;

  }else if( compareClassGuid( p2_guid ) )
    {
         std::unique_ptr< CombinedMuonFeatureContainer_p2 > col_vect( poolReadObject< CombinedMuonFeatureContainer_p2 >() );
	 //         std::cout << "Reading CMFC p2" << std::endl;
         return m_converter2.createTransient( col_vect.get(), mlog ) ;

  }else if( compareClassGuid( tlp1_guid ) )     
    {
         std::unique_ptr< CombinedMuonFeatureContainer_tlp1 > col_vect( poolReadObject< CombinedMuonFeatureContainer_tlp1 >() );
	 //  std::cout << "Reading CMFC tlp1" << std::endl;
         return m_converter1.createTransient( col_vect.get(), mlog );
      
    }else if( compareClassGuid( p0_guid ) ){
      
      return poolReadObject< CombinedMuonFeatureContainer >();
      
    }else  throw std::runtime_error( "Unsupported persistent version of CombinedMuonFeatureContainer" );
  
}//end of create transient method

