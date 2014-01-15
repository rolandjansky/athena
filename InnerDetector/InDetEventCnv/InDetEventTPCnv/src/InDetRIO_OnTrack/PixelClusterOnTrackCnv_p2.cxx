/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#undef private
#undef protected

#include "InDetEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrackCnv_p2.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"


void PixelClusterOnTrackCnv_p2::persToTrans( const InDet::PixelClusterOnTrack_p2 *persObj,InDet::PixelClusterOnTrack *transObj, MsgStream &log ){
    //std::cout<<"READING PixelClusterOnTrackCnv_p2"<<std::endl;

    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize PixelClusterOnTrackCnv_p2 " << endreq;
        }
    }


    transObj->m_identifier = m_pixId->pixel_id(persObj->m_id);
  
    //    transObj->m_identifier = persObj->m_id;

    //transObj->m_localParams = createTransFromPStore( &m_localParCnv, persObj->m_localParams, log );
    //transObj->m_localErrMat = createTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, log );
    fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &transObj->m_localParams, log );
    Trk::ErrorMatrix dummy;
    fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
    EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "PixelClusterOnTrackCnv_p2");
    transObj->m_idDE = persObj->m_idDE;
    transObj->m_isbroad = persObj->m_isbroad;

    transObj->m_hasClusterAmbiguity = persObj->m_hasClusterAmbiguity;
    transObj->m_isFake              = persObj->m_isFake;
    transObj->m_energyLoss          = persObj->m_energyLoss;
    m_elCnv.persToTrans(&persObj->m_prdLink,&transObj->m_rio,log);  

    // Attempt to call supertool to fill in detElements
    m_eventCnvTool->recreateRIO_OnTrack(transObj);
    if (transObj->detectorElement()==0) 
        log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
            << "probably because of a problem with the Identifier/IdentifierHash : ("
            << transObj->identify()<<"/"<<transObj->idDE()<<endreq;

   //std::cout << "PixelClusterOnTrackCnv_p2::persToTrans() "
   //          << transObj->m_identifier
   //          << " [" << transObj->m_rio.dataID() << "/" << transObj->m_rio.index() << "]"
   //          << " " << persObj->m_prdLink.m_contName
   //          << " (" << persObj->m_prdLink.m_elementIndex
   //          << ")" << std::endl;
}


void PixelClusterOnTrackCnv_p2::transToPers( const InDet::PixelClusterOnTrack *transObj, InDet::PixelClusterOnTrack_p2 *persObj, MsgStream &log ) {

    const Trk::RIO_OnTrack* tobj = static_cast<const Trk::RIO_OnTrack *>(transObj);
    m_eventCnvTool->prepareRIO_OnTrack(const_cast<Trk::RIO_OnTrack *>(tobj));
    //m_eventCnvTool->prepareRIO_OnTrack(const_cast<Trk::RIO_OnTrack *>(transObj));
    persObj->m_id = transObj->m_identifier.get_compact();
    persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->m_localParams, log );
    Trk::ErrorMatrix pMat;
    EigenHelpers::eigenMatrixToVector(pMat.values, transObj->m_localCovariance, "PixelClusterOnTrackCnv_p2");
    persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );

    persObj->m_idDE = transObj->m_idDE;
    persObj->m_isbroad = transObj->m_isbroad;

    persObj->m_hasClusterAmbiguity = transObj->m_hasClusterAmbiguity;
    persObj->m_isFake              = transObj->m_isFake;
    persObj->m_energyLoss          = transObj->m_energyLoss;

    if(!transObj->m_rio.dataID().empty()){
        persObj->m_prdLink.m_contName          = transObj->m_rio.dataID();
        persObj->m_prdLink.m_elementIndex     = transObj->m_rio.index();
    }else{
        persObj->m_prdLink.m_contName         = "";
        persObj->m_prdLink.m_elementIndex     = 0;
    }

   //std::cout << "PixelClusterOnTrackCnv_p2::transToPers() "
   //          << transObj->m_identifier
   //          << " [" << transObj->m_rio.dataID() << "/" << transObj->m_rio.index() << "]"
   //          << ": " << persObj->m_prdLink.m_contName
   //          << " (" << persObj->m_prdLink.m_elementIndex
   //          << ")" << std::endl;
   //transObj->dump(std::cout) << std::endl;
}

StatusCode PixelClusterOnTrackCnv_p2::initialize(MsgStream &log) {
    // Do not initialize again:
    m_isInitialized=true;

   //std::cout << "PixelClusterOnTrackCnv_p2::initialize() begin" << std::endl;

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


   //std::cout << "PixelClusterOnTrackCnv_p2::initialize() end" << std::endl;

    //    if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endreq;
    return StatusCode::SUCCESS;
}


