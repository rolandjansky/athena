/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FTK_RawTrackContainerCnv.h"

#include "TrigFTK_RawDataTPCnv/FTK_RawTrackContainerCnv_tlp1.h"
#include "TrigFTK_RawDataTPCnv/FTK_RawTrackContainer_tlp1.h"

//constructor
FTK_RawTrackContainerCnv::FTK_RawTrackContainerCnv( ISvcLocator* svcloc ) :
  FTK_RawTrackContainerCnvBase( svcloc ),
  m_tlp_Converter (new FTK_RawTrackContainerCnv_PERS)
{}

//destructor
FTK_RawTrackContainerCnv::~FTK_RawTrackContainerCnv()
{
  delete m_tlp_Converter;
}


//create persistent
FTK_RawTrackContainer_PERS* FTK_RawTrackContainerCnv::createPersistent(FTK_RawTrackContainer* transObj) {
    MsgStream mlog(messageService(), "FTK_RawTrackContainerConverter" );
    mlog << MSG::DEBUG << "FTK_RawTrackContainerCnv::createPersistent" << endreq;

    FTK_RawTrackContainer_PERS* persObj = m_tlp_Converter->createPersistent( transObj, mlog );

    return persObj;
}

//create transient
FTK_RawTrackContainer* FTK_RawTrackContainerCnv::createTransient() {
  
  MsgStream mlog(messageService(), "FTK_RawTrackContainerConverter" );
  mlog << MSG::DEBUG << "FTK_RawTrackContainerCnv::createTransient " << endreq;

  static pool::Guid tlp1_guid("C1A5436E-8520-4399-82FC-E6828B19688D");
  
  FTK_RawTrackContainer  *transObj = 0;
  if( compareClassGuid(tlp1_guid) ) {
    
    mlog << MSG::DEBUG << "FTK_RawTrackContainerCnv::reading tlp1 persistent object" << endreq;
    FTK_RawTrackContainerCnv_tlp1 tlp1_Converter;
    poolReadObject< FTK_RawTrackContainer_tlp1 >( tlp1_Converter );
    transObj = tlp1_Converter.createTransient( mlog );
  } else {
    throw std::runtime_error("Unsupported persistent version of Data container");
  }
  
  return transObj;
}
