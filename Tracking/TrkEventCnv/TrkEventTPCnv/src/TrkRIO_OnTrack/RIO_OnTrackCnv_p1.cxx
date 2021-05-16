/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventTPCnv/TrkRIO_OnTrack/RIO_OnTrackCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void RIO_OnTrackCnv_p1::persToTrans( const Trk::RIO_OnTrack_p1 *persObj, Trk::RIO_OnTrack *transObj, MsgStream &log )
{
//   static int count = 0;
//   if( count++ < 10 ) {
//      log << MSG::INFO << ">>>>---------  RIO_OnTrack ID = " << persObj->m_id << endmsg;
//   }

  //transObj->m_identifier = persObj->m_id;

  if(!m_isInitialized) {
    if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize RIO_OnTRackCnv_p1 " << endmsg;
    }
  }

  if (m_pixId->is_shortened_pixel_id(persObj->m_id)) {
    transObj->m_identifier = m_pixId->pixel_id_from_shortened(persObj->m_id);
  }
  else {
    transObj->m_identifier = persObj->m_id;
  }

  fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &transObj->m_localParams, log );

  //Error matrix
  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "RIO_OnTrackCnv_p1");
  // Now try to fill actual data... Amg::MatrixX     m_localCovariance;
  // 1x1 => size=1
  // 2x2 => size=3
  size_t size = dummy.values.size();
  if (size==1){
    transObj->m_localCovariance<<dummy.values[0];
  } else if (size==3){
    transObj->m_localCovariance<<dummy.values[0],dummy.values[1],dummy.values[1],dummy.values[2];
  } else {
    throw std::runtime_error("RIO_OnTrackCnv_p2::persToTrans - matrix has unexpected size of "+std::to_string (size)+"!");
  }
  
   // Attempt to call supertool to fill in detElements

  m_eventCnvTool->recreateRIO_OnTrack(transObj);
//   std::cout<<"RIO_OnTrackCnv_p1::persToTrans after  : detEl="<<transObj->detectorElement()<<std::endl;
  if (transObj->detectorElement()==nullptr) 
    log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
    << "probably because of a problem with the Identifier/IdentifierHash : ("
    << transObj->identify()<<"/"<<transObj->idDE()<<endmsg;
}


void RIO_OnTrackCnv_p1::transToPers( const Trk::RIO_OnTrack * /**transObj*/, Trk::RIO_OnTrack_p1 * /**persObj*/, MsgStream & /**log*/ )
{
  throw std::runtime_error("RIO_OnTrackCnv_p1::transToPers is deprecated!"); 
}

StatusCode RIO_OnTrackCnv_p1::initialize(MsgStream &log) {
   // Do not initialize again:
  m_isInitialized=true;

   // Get Storegate, ID helpers, and so on
  ISvcLocator* svcLocator = Gaudi::svcLocator();
   // get StoreGate service
  StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
  if (sc.isFailure()) {
    log << MSG::FATAL << "StoreGate service not found !" << endmsg;
    return StatusCode::FAILURE;
  }

   // get DetectorStore service
  StoreGateSvc *detStore;
  sc = svcLocator->service("DetectorStore", detStore);
  if (sc.isFailure()) {
    log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;
  } 
   //   else {
   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endmsg;
   //   }

   // Get the sct helper from the detector store
  sc = detStore->retrieve(m_pixId, "PixelID");
  if (sc.isFailure()) {
    log << MSG::FATAL << "Could not get PixelID helper !" << endmsg;
    return StatusCode::FAILURE;
  } 


   //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;
  return StatusCode::SUCCESS;
}
