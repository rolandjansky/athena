/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonFeatureContainerCnv.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_tlp1.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_tlp2.h"

#include "TrigMuonEventTPCnv/MuonFeatureContainer_p3.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainerCnv_p3.h"

static MuonFeatureContainerCnv_tlp1   TPconverter1;
static MuonFeatureContainerCnv_tlp2   TPconverter2;
static MuonFeatureContainerCnv_p3     TPconverter;

//createPersistent 
MuonFeatureContainer_PERS * MuonFeatureContainerCnv::createPersistent( MuonFeatureContainer *transObj)
{
  MsgStream mlog(messageService(), "MuonFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "MuonFeatureContainerCnv::createPersistent called" << endreq;
  
  MuonFeatureContainer_PERS * p_cont = TPconverter.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
MuonFeatureContainer * MuonFeatureContainerCnv::createTransient()
{
  MsgStream mlog(messageService(), "MuonFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "MuonFeatureContainerCnv::createTransient called" << endreq;
  
  static pool::Guid p3_guid( "5B571BCD-FE49-4C56-A357-1B535FE65829" );
  static pool::Guid tlp2_guid( "2F4ABBC8-EA77-487B-820F-76179BB3828C" );
  static pool::Guid tlp1_guid( "039BE61C-DE27-48B3-A2AE-7172BB755CEE" );
  static pool::Guid p0_guid( "45225F26-A517-4E8B-BA93-DDFD1217B9A8" );
  
  if( compareClassGuid( tlp1_guid ) )     
  {
         std::auto_ptr< MuonFeatureContainer_tlp1 > col_vect( poolReadObject< MuonFeatureContainer_tlp1 >() );
	 //         std::cout << "Reading MFC p1" << std::endl;
         return TPconverter1.createTransient( col_vect.get(), mlog ) ;
  }
  else if( compareClassGuid( tlp2_guid ) )     
  {
         std::auto_ptr< MuonFeatureContainer_tlp2 > col_vect( poolReadObject< MuonFeatureContainer_tlp2 >() );
	 // std::cout << "Reading MFC p2" << std::endl;
         return TPconverter2.createTransient( col_vect.get(), mlog ) ;
  }
  else if( compareClassGuid( p3_guid ) )     
  {
         std::auto_ptr< MuonFeatureContainer_p3 > col_vect( poolReadObject< MuonFeatureContainer_p3 >() );
	 // std::cout << "Reading MFC p3" << std::endl;
         return TPconverter.createTransient( col_vect.get(), mlog ) ;
  }
  else if( compareClassGuid( p0_guid ) )
  {
         return poolReadObject< MuonFeatureContainer >();
  }
  else  throw std::runtime_error( "Unsupported persistent version of MuonFeatureContainer" );
  
}//end of create transient method

