/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetTrackCollectionCnv.h"


#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp4.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_tlp4.h"

#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp3.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_tlp3.h"

#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp2.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_tlp2.h"

#include "TrigInDetEventTPCnv/TrigInDetTrackCollectionCnv_tlp1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCollection_tlp1.h"

//constructor
TrigInDetTrackCollectionCnv::TrigInDetTrackCollectionCnv( ISvcLocator* svcloc ) :
  TrigInDetTrackCollectionCnvBase( svcloc ),
  m_tlp_Converter (new TrigInDetTrackCollectionCnv_PERS)
{}

//destructor
TrigInDetTrackCollectionCnv::~TrigInDetTrackCollectionCnv()
{
  delete m_tlp_Converter;
}


//create persistent
TrigInDetTrackCollection_PERS* TrigInDetTrackCollectionCnv::createPersistent(TrigInDetTrackCollection* transObj) {
    MsgStream mlog(messageService(), "TrigInDetTrackCollectionConverter" );
    mlog << MSG::DEBUG << "TrigInDetTrackCollectionCnv::createPersistent" << endreq;

    TrigInDetTrackCollection_PERS* persObj = m_tlp_Converter->createPersistent( transObj, mlog );

    return persObj;
}

//create transient
TrigInDetTrackCollection* TrigInDetTrackCollectionCnv::createTransient() {
  
  MsgStream mlog(messageService(), "TrigInDetTrackCollectionConverter" );
  mlog << MSG::DEBUG << "TrigInDetTrackCollectionCnv::createTransient " << endreq;

  static pool::Guid tlp4_guid("E1B8EE19-4F7B-4EF2-8FDC-45AA871BD628");
  static pool::Guid tlp3_guid("196F811B-382D-47A9-8551-D2038343BFFA");
  static pool::Guid tlp2_guid("4DBBB46F-33C9-4571-AE5E-A2335D7A2976");
  static pool::Guid tlp1_guid("0B5CA4CA-0A0F-43DC-8973-0E2212B2E8DA");
  static pool::Guid p0_guid("5F6029E6-764B-4126-891D-73BFC3CC391C");
  
  TrigInDetTrackCollection  *transObj = 0;
  if( compareClassGuid(tlp4_guid) ) {
    
    mlog << MSG::DEBUG << "TrigInDetTrackCollectionCnv::reading tlp4 persistent object" << endreq;
    TrigInDetTrackCollectionCnv_tlp4 tlp4_Converter;
    poolReadObject< TrigInDetTrackCollection_tlp4 >( tlp4_Converter );
    transObj = tlp4_Converter.createTransient( mlog );
  } else  if( compareClassGuid(tlp3_guid) ) {
    
    mlog << MSG::DEBUG << "TrigInDetTrackCollectionCnv::reading tlp3 persistent object" << endreq;
    TrigInDetTrackCollectionCnv_tlp3 tlp3_Converter;
    poolReadObject< TrigInDetTrackCollection_tlp3 >( tlp3_Converter );
    transObj = tlp3_Converter.createTransient( mlog );
  } else if( compareClassGuid(tlp2_guid) ) {
    
    mlog << MSG::DEBUG << "TrigInDetTrackCollectionCnv::reading tlp2 persistent object" << endreq;
    TrigInDetTrackCollectionCnv_tlp2 tlp2_Converter;
    poolReadObject< TrigInDetTrackCollection_tlp2 >( tlp2_Converter );
    transObj = tlp2_Converter.createTransient( mlog );
  } else if( compareClassGuid(tlp1_guid) ) {
      
    mlog << MSG::DEBUG << "TrigInDetTrackCollectionCnv::reading tlp1 persistent object" << endreq;
    TrigInDetTrackCollectionCnv_tlp1 tlp1_Converter;
    poolReadObject< TrigInDetTrackCollection_tlp1 >( tlp1_Converter );
    transObj = tlp1_Converter.createTransient( mlog );
    
  }
  
  else if( compareClassGuid(p0_guid) ) {
    
    mlog << MSG::DEBUG << "TrigInDetTrackCollectionCnv::reading p0 persistent object" << endreq;
    // old version from before TP separation, just return it
    transObj = this->poolReadObject<TrigInDetTrackCollection>();
  }  
  else {
    throw std::runtime_error("Unsupported persistent version of Data container");
  }
  
  return transObj;
}
