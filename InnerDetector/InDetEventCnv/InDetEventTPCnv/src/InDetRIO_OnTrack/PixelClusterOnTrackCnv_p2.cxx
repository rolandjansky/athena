/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrackCnv_p2.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"
#include "AthenaKernel/errorcheck.h"


void PixelClusterOnTrackCnv_p2::persToTrans( const InDet::PixelClusterOnTrack_p2 *persObj,InDet::PixelClusterOnTrack *transObj, MsgStream &log ){
    //std::cout<<"READING PixelClusterOnTrackCnv_p2"<<std::endl;

    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize PixelClusterOnTrackCnv_p2 " << endreq;
        }
    }

    ElementLinkToIDCPixelClusterContainer rio;
    m_elCnv.persToTrans(&persObj->m_prdLink,&rio,log);  

    Trk::LocalParameters localParams;
    fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &localParams, log );

    Trk::ErrorMatrix dummy;
    Amg::MatrixX localCovariance;
    fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
    EigenHelpers::vectorToEigenMatrix(dummy.values, localCovariance, "PixelClusterOnTrackCnv_p2");

    *transObj = InDet::PixelClusterOnTrack(rio,
                                           localParams,
                                           localCovariance,
                                           persObj->m_idDE,
                                           m_pixId->pixel_id(persObj->m_id),
                                           persObj->m_energyLoss,
                                           persObj->m_isFake,
                                           persObj->m_hasClusterAmbiguity,
                                           persObj->m_isbroad
                                           );

    // Attempt to call supertool to fill in detElements
    m_eventCnvTool->recreateRIO_OnTrack(transObj);
    if (transObj->detectorElement()==0) 
        log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
            << "probably because of a problem with the Identifier/IdentifierHash : ("
            << transObj->identify()<<"/"<<transObj->idDE()<<endreq;

}


void PixelClusterOnTrackCnv_p2::transToPers( const InDet::PixelClusterOnTrack *transObj, InDet::PixelClusterOnTrack_p2 *persObj, MsgStream &log ) {

    const Trk::RIO_OnTrack* tobj = static_cast<const Trk::RIO_OnTrack *>(transObj);
    m_eventCnvTool->prepareRIO_OnTrack(const_cast<Trk::RIO_OnTrack *>(tobj));
    //m_eventCnvTool->prepareRIO_OnTrack(const_cast<Trk::RIO_OnTrack *>(transObj));
    persObj->m_id = transObj->identify().get_compact();
    persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->localParameters(), log );
    Trk::ErrorMatrix pMat;
    EigenHelpers::eigenMatrixToVector(pMat.values, transObj->localCovariance(), "PixelClusterOnTrackCnv_p2");
    persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );

    persObj->m_idDE = transObj->idDE();
    persObj->m_isbroad = transObj->isBroadCluster();

    persObj->m_hasClusterAmbiguity = transObj->hasClusterAmbiguity();
    persObj->m_isFake              = transObj->isFake();
    persObj->m_energyLoss          = transObj->energyLoss();

    if(!transObj->prepRawDataLink().dataID().empty()){
      persObj->m_prdLink.m_contName          = transObj->prepRawDataLink().dataID();
      persObj->m_prdLink.m_elementIndex     = transObj->prepRawDataLink().index();
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

StatusCode PixelClusterOnTrackCnv_p2::initialize(MsgStream &/*log*/) {
    // Do not initialize again:
    m_isInitialized=true;

   //std::cout << "PixelClusterOnTrackCnv_p2::initialize() begin" << std::endl;

    // Get Storegate, ID helpers, and so on
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StoreGateSvc *detStore;
    CHECK( svcLocator->service("DetectorStore", detStore) );
    CHECK(  detStore->retrieve(m_pixId, "PixelID") );

    return StatusCode::SUCCESS;
}


