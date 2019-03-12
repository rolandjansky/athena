/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrackCnv_p2.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"
#include "AthenaKernel/errorcheck.h"
#include "TrkEventTPCnv/TrkEventPrimitives/ErrorMatrixCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/LocalParametersCnv_p1.h"
#include "InDetIdentifier/PixelID.h"

void PixelClusterOnTrackCnv_p2::persToTrans( const InDet::PixelClusterOnTrack_p2 *persObj,InDet::PixelClusterOnTrack *transObj, MsgStream &log ){
    //std::cout<<"READING PixelClusterOnTrackCnv_p2"<<std::endl;

    if(!m_isInitialized) {
        if (this->initialize(log) != StatusCode::SUCCESS) {
            log << MSG::FATAL << "Could not initialize PixelClusterOnTrackCnv_p2 " << endmsg;
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
            << transObj->identify()<<"/"<<transObj->idDE()<<endmsg;

}


void PixelClusterOnTrackCnv_p2::transToPers( const InDet::PixelClusterOnTrack *transObj, InDet::PixelClusterOnTrack_p2 *persObj, MsgStream &log ) {
  if (transObj==nullptr or persObj==nullptr) return;

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

  const std::string pixelClusContName{"PixelClusters"};
  ElementLink<InDet::PixelClusterContainer>::index_type hashAndIndex{0};
  bool isFound{m_eventCnvTool->getHashAndIndex<InDet::PixelClusterContainer, InDet::PixelClusterOnTrack>(transObj, pixelClusContName, hashAndIndex)};
  persObj->m_prdLink.m_contName = (isFound ? pixelClusContName : "");
  persObj->m_prdLink.m_elementIndex = hashAndIndex;
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


