/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   PixelClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#undef private
#undef protected

#include "InDetEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrackCnv_p1.h"


void PixelClusterOnTrackCnv_p1::
persToTrans( const InDet::PixelClusterOnTrack_p1 *persObj,
	     InDet::PixelClusterOnTrack *transObj, MsgStream &log )
{
   fillTransFromPStore( &m_scCnv, persObj->m_RIO,  transObj, log );

   if(!m_isInitialized) {
     if (this->initialize(log) != StatusCode::SUCCESS) {
       log << MSG::FATAL << "Could not initialize PixelClusterOnTrackCnv_p1 " << endreq;
     }
   }

//    transObj->m_idDE = persObj->m_idDE;
   
   transObj->m_hasClusterAmbiguity = persObj->m_hasClusterAmbiguity;
   transObj->m_isFake              = persObj->m_isFake;
   transObj->m_energyLoss          = persObj->m_energyLoss;
//   m_elementLinkConverter.resetForCnv(persObj->m_elementLinkToIDCPixelClusterContainerNames);
//   m_elementLinkConverter.persToTrans(&persObj->m_elementLinkToIDCPixelClusterContainer,&transObj->m_rio,log);
   m_elCnv.persToTrans(&persObj->m_prdLink,&transObj->m_rio,log);  
}


void PixelClusterOnTrackCnv_p1::
transToPers( const InDet::PixelClusterOnTrack *transObj,
	     InDet::PixelClusterOnTrack_p1 *persObj, MsgStream &log ) 
{
   persObj->m_RIO = baseToPersistent( &m_scCnv,  transObj, log );
//    persObj->m_idDE = transObj->m_idDE;
   
   persObj->m_hasClusterAmbiguity = transObj->m_hasClusterAmbiguity;
   persObj->m_isFake              = transObj->m_isFake;
   persObj->m_energyLoss          = transObj->m_energyLoss;
//   m_elementLinkConverter.resetForCnv(persObj->m_elementLinkToIDCPixelClusterContainerNames);
//   m_elementLinkConverter.transToPers(&transObj->m_rio,&persObj->m_elementLinkToIDCPixelClusterContainer,log);
   // m_elCnv.transToPers(&transObj->m_rio,&persObj->m_prdLink,log);  
   persObj->m_prdLink.m_contName          = transObj->m_rio.dataID();// New suggestion from RD - above crashes
   persObj->m_prdLink.m_elementIndex     = transObj->m_rio.index();
}

StatusCode PixelClusterOnTrackCnv_p1::initialize(MsgStream &log) {
   // Do not initialize again:
   m_isInitialized=true;

   // Get Storegate, ID helpers, and so on
   ISvcLocator* svcLocator = Gaudi::svcLocator();
   // get StoreGate service
   StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
   if (sc.isFailure()) {
      log << MSG::FATAL << "StoreGate service not found !" << endreq;
      return StatusCode::FAILURE;
   }

   // get DetectorStore service
   StoreGateSvc *detStore;
   sc = svcLocator->service("DetectorStore", detStore);
   if (sc.isFailure()) {
      log << MSG::FATAL << "DetectorStore service not found !" << endreq;
      return StatusCode::FAILURE;
   } 
   //   else {
   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endreq;
   //   }

   // Get the sct helper from the detector store
   sc = detStore->retrieve(m_pixId, "PixelID");
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get PixelID helper !" << endreq;
      return StatusCode::FAILURE;
   } 


   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endreq;
   return StatusCode::SUCCESS;
}

