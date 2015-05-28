/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IsoMuonFeatureContainerCnv.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_tlp1.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainerCnv_tlp1.h"

#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p2.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainerCnv_p2.h"

#include "TrigMuonEventTPCnv/IsoMuonFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/IsoMuonFeatureContainerCnv_p3.h"

static IsoMuonFeatureContainerCnv_tlp1   TLPconverter1;
static IsoMuonFeatureContainerCnv_p2     TPconverter2;
static IsoMuonFeatureContainerCnv_p3     TPconverter;

//createPersistent 
IsoMuonFeatureContainer_PERS * IsoMuonFeatureContainerCnv::createPersistent( IsoMuonFeatureContainer *transObj)
{
  MsgStream mlog(messageService(), "IsoMuonFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "IsoMuonFeatureContainerCnv::createPersistent called" << endreq;
  
  IsoMuonFeatureContainer_PERS * p_cont = TPconverter.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method

//createTransient
IsoMuonFeatureContainer * IsoMuonFeatureContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "IsoMuonFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "IsoMuonFeatureContainerCnv::createTransient called" << endreq;
  
  static pool::Guid p3_guid( "291897DE-5380-424C-AE8F-FFE69DCC8F15" );  
  static pool::Guid p2_guid( "A9A660B1-DEFD-4B56-A10D-9F41178715BD" );  
  static pool::Guid tlp1_guid( "B9543660-E776-405D-8DB7-06AD06A24BAB" );
  static pool::Guid p0_guid( "3962B221-2A36-4160-AEE5-3BB6BC29BB46" );

  //IsoMuonFeatureContainer *p_collection = 0;
  if( compareClassGuid( p3_guid ) ){
         std::auto_ptr< IsoMuonFeatureContainer_p3 > col_vect( poolReadObject< IsoMuonFeatureContainer_p3 >() );
	 //         std::cout << "Reading IMFC p3" << std::endl;
         return TPconverter.createTransient( col_vect.get(), mlog ) ;

    }else if( compareClassGuid( p2_guid ) ){
         std::auto_ptr< IsoMuonFeatureContainer_p2 > col_vect( poolReadObject< IsoMuonFeatureContainer_p2 >() );
	 //         std::cout << "Reading IMFC p2" << std::endl;
         return TPconverter2.createTransient( col_vect.get(), mlog ) ;

    }else if( compareClassGuid( tlp1_guid ) ){
         std::auto_ptr< IsoMuonFeatureContainer_tlp1 > col_vect( poolReadObject< IsoMuonFeatureContainer_tlp1 >() );
	 //  std::cout << "Reading IMFC tlp1" << std::endl;
         return TLPconverter1.createTransient( col_vect.get(), mlog );
      
    }else if( compareClassGuid( p0_guid ) ){
         return poolReadObject< IsoMuonFeatureContainer >();
      
    }else  throw std::runtime_error( "Unsupported persistent version of IsoMuonFeatureContainer" );
  
}//end of create transient method

