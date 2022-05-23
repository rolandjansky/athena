/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonFeatureContainerCnv.h"
#include "TrigMuonEventTPCnv/MuonFeatureContainer_p3.h"

//createPersistent 
MuonFeatureContainer_PERS * MuonFeatureContainerCnv::createPersistent( MuonFeatureContainer *transObj)
{
  MsgStream mlog(msgSvc(), "MuonFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "MuonFeatureContainerCnv::createPersistent called" << endmsg;
  
  MuonFeatureContainer_PERS * p_cont = m_converter.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
MuonFeatureContainer * MuonFeatureContainerCnv::createTransient()
{
  MsgStream mlog(msgSvc(), "MuonFeatureContainerConverter" );
  
  mlog << MSG::DEBUG << "MuonFeatureContainerCnv::createTransient called" << endmsg;
  
  static const pool::Guid p3_guid( "5B571BCD-FE49-4C56-A357-1B535FE65829" );
  static const pool::Guid tlp2_guid( "2F4ABBC8-EA77-487B-820F-76179BB3828C" );
  static const pool::Guid tlp1_guid( "039BE61C-DE27-48B3-A2AE-7172BB755CEE" );
  static const pool::Guid p0_guid( "45225F26-A517-4E8B-BA93-DDFD1217B9A8" );
  
  if( compareClassGuid( tlp1_guid ) )     
  {
         std::unique_ptr< MuonFeatureContainer_tlp1 > col_vect( poolReadObject< MuonFeatureContainer_tlp1 >() );
	 //         std::cout << "Reading MFC p1" << std::endl;
         return m_converter1.createTransient( col_vect.get(), mlog ) ;
  }
  else if( compareClassGuid( tlp2_guid ) )     
  {
         std::unique_ptr< MuonFeatureContainer_tlp2 > col_vect( poolReadObject< MuonFeatureContainer_tlp2 >() );
	 // std::cout << "Reading MFC p2" << std::endl;
         return m_converter2.createTransient( col_vect.get(), mlog ) ;
  }
  else if( compareClassGuid( p3_guid ) )     
  {
         std::unique_ptr< MuonFeatureContainer_p3 > col_vect( poolReadObject< MuonFeatureContainer_p3 >() );
	 // std::cout << "Reading MFC p3" << std::endl;
         return m_converter.createTransient( col_vect.get(), mlog ) ;
  }
  else if( compareClassGuid( p0_guid ) )
  {
         return poolReadObject< MuonFeatureContainer >();
  }
  else  throw std::runtime_error( "Unsupported persistent version of MuonFeatureContainer" );
  
}//end of create transient method

