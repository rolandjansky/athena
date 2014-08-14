/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CombinedMuonFeatureContainerCnv.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_tlp1.h"

#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_p2.h"

#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_p3.h"

#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainer_p4.h"
#include "TrigMuonEventTPCnv/CombinedMuonFeatureContainerCnv_p4.h"

static CombinedMuonFeatureContainerCnv_tlp1   TLPconverter1;
static CombinedMuonFeatureContainerCnv_p2     TPconverter2;
static CombinedMuonFeatureContainerCnv_p3     TPconverter3;
static CombinedMuonFeatureContainerCnv_p4     TPconverter;

//createPersistent 
CombinedMuonFeatureContainer_PERS * CombinedMuonFeatureContainerCnv::createPersistent( CombinedMuonFeatureContainer *transObj)
{
  MsgStream mlog(messageService(), "CombinedMuonFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "CombinedMuonFeatureContainerCnv::createPersistent called" << endreq;
  
  CombinedMuonFeatureContainer_PERS * p_cont = TPconverter.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
CombinedMuonFeatureContainer * CombinedMuonFeatureContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "CombinedMuonFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "CombinedMuonFeatureContainerCnv::createTransient called" << endreq;
  
  static pool::Guid p4_guid( "A06B9B51-FFA9-4AC1-9079-8AF49C4A9B87" );
  static pool::Guid p3_guid( "02262E5F-7645-4919-9CD9-66D88796DCFA" );
  static pool::Guid p2_guid( "E9D9F99E-D64F-4114-90EA-236FAF9063BB" );
  static pool::Guid tlp1_guid( "9C558B18-1B1D-4186-AF26-A5F3EACCBE1B" );
  static pool::Guid p0_guid( "574BFA1C-ADB8-40DB-B538-0CA20E20CAAD" );
  
  //CombinedMuonFeatureContainer *p_collection = 0;
  if( compareClassGuid( p4_guid ) ){
         std::auto_ptr< CombinedMuonFeatureContainer_p4 > col_vect( poolReadObject< CombinedMuonFeatureContainer_p4 >() );
	 //         std::cout << "Reading CMFC p2" << std::endl;
         return TPconverter.createTransient( col_vect.get(), mlog ) ;

  }else if( compareClassGuid( p3_guid ) ){
         std::auto_ptr< CombinedMuonFeatureContainer_p3 > col_vect( poolReadObject< CombinedMuonFeatureContainer_p3 >() );
	 //         std::cout << "Reading CMFC p2" << std::endl;
         return TPconverter3.createTransient( col_vect.get(), mlog ) ;

  }else if( compareClassGuid( p2_guid ) )
    {
         std::auto_ptr< CombinedMuonFeatureContainer_p2 > col_vect( poolReadObject< CombinedMuonFeatureContainer_p2 >() );
	 //         std::cout << "Reading CMFC p2" << std::endl;
         return TPconverter2.createTransient( col_vect.get(), mlog ) ;

  }else if( compareClassGuid( tlp1_guid ) )     
    {
         std::auto_ptr< CombinedMuonFeatureContainer_tlp1 > col_vect( poolReadObject< CombinedMuonFeatureContainer_tlp1 >() );
	 //  std::cout << "Reading CMFC tlp1" << std::endl;
         return TLPconverter1.createTransient( col_vect.get(), mlog );
      
    }else if( compareClassGuid( p0_guid ) ){
      
      return poolReadObject< CombinedMuonFeatureContainer >();
      
    }else  throw std::runtime_error( "Unsupported persistent version of CombinedMuonFeatureContainer" );
  
}//end of create transient method

