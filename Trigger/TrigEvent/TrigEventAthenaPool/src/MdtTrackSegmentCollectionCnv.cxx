/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtTrackSegmentCollectionCnv.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_tlp1.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollectionCnv_tlp1.h"

#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_p2.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollectionCnv_p2.h"

static MdtTrackSegmentCollectionCnv_tlp1   TLPconverter1;
static MdtTrackSegmentCollectionCnv_p2     TPconverter;

//createPersistent 
MdtTrackSegmentCollection_PERS * MdtTrackSegmentCollectionCnv::createPersistent( MdtTrackSegmentCollection *transObj)
{
  MsgStream mlog(messageService(), "MdtTrackSegmentCollectionConverter" );
  
  mlog << MSG::DEBUG << "MdtTrackSegmentCollectionCnv::createPersistent called" << endreq;
  
  MdtTrackSegmentCollection_PERS * p_cont = TPconverter.createPersistent( transObj, mlog );
  
  return p_cont;
  
}//end of create persistent method


//createTransient
MdtTrackSegmentCollection * MdtTrackSegmentCollectionCnv::createTransient()
{
  MsgStream mlog(messageService(), "MdtTrackSegmentCollectionConverter" );
  
  mlog << MSG::DEBUG << "MdtTrackSegmentCollectionCnv::createTransient called" << endreq;
  
  static pool::Guid p2_guid( "18B6EEC4-8022-45A3-A31F-90EA6E6E9925" );
  static pool::Guid tlp1_guid( "6A4584AD-B7A5-4B0F-BF29-DC7CD75319D5" );
  static pool::Guid p0_guid( "EBCB8B2A-9689-43B6-8B30-6BFE503F25F3" );
  
  if( compareClassGuid( p2_guid ) ) {
     std::auto_ptr< MdtTrackSegmentCollection_p2 > col_vect( poolReadObject< MdtTrackSegmentCollection_p2 >() );
     return TPconverter.createTransient( col_vect.get(), mlog ) ;

  }
  else if( compareClassGuid( tlp1_guid ) ) {
     std::auto_ptr< MdtTrackSegmentCollection_tlp1 > col_vect( poolReadObject< MdtTrackSegmentCollection_tlp1 >() );
     return TLPconverter1.createTransient( col_vect.get(), mlog );

  }
  else if( compareClassGuid( p0_guid ) ) {
     return poolReadObject< MdtTrackSegmentCollection >();
      
  }
  else  throw std::runtime_error( "Unsupported persistent version of MdtTrackSegmentCollection" );
  
}//end of create transient method

