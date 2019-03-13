/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircleOnTrackCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/TRT_DriftCircleOnTrackCnv_p2.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"


void TRT_DriftCircleOnTrackCnv_p2::persToTrans( const InDet::TRT_DriftCircleOnTrack_p2 *persObj, InDet::TRT_DriftCircleOnTrack *transObj, MsgStream &log ) {

    ElementLinkToIDCTRT_DriftCircleContainer rio;
    m_elCnv.persToTrans(&persObj->m_prdLink,&rio,log);  

    Trk::LocalParameters localParams;
    fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &localParams, log );

    Trk::ErrorMatrix dummy;
    Amg::MatrixX localCovariance;
    fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
    EigenHelpers::vectorToEigenMatrix(dummy.values, localCovariance, "TRT_DriftCircleOnTrackCnv_p2");

    *transObj = InDet::TRT_DriftCircleOnTrack (rio,
                                               localParams,
                                               localCovariance,
                                               persObj->m_idDE,
                                               Identifier(persObj->m_id),
                                               persObj->m_positionAlongWire,
                                               persObj->m_localAngle,
                                               static_cast<Trk::DriftCircleStatus>( persObj->m_status ),
                                               persObj->m_highLevel,
                                               persObj->m_timeOverThreshold
                                               );

    m_eventCnvTool->recreateRIO_OnTrack(transObj);
    if (transObj->detectorElement()==0) 
        log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
            << "probably because of a problem with the Identifier/IdentifierHash : ("
            << transObj->identify()<<"/"<<transObj->idDE()<<endmsg;

}


void TRT_DriftCircleOnTrackCnv_p2::transToPers( const InDet::TRT_DriftCircleOnTrack    *transObj, InDet::TRT_DriftCircleOnTrack_p2 *persObj, MsgStream &log) {
  if (transObj==nullptr or persObj==nullptr) return;

  persObj->m_id = transObj->identify().get_identifier32().get_compact();
  persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->localParameters(), log );
  Trk::ErrorMatrix pMat;
  EigenHelpers::eigenMatrixToVector(pMat.values, transObj->localCovariance(), "TRT_DriftCircleOnTrackCnv_p2");
  persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );
  persObj->m_idDE      = transObj->idDE();
  persObj->m_status    = static_cast<unsigned int>( transObj->status() );
  persObj->m_highLevel = transObj->highLevel();
  persObj->m_localAngle	       = transObj->localAngle();
  persObj->m_positionAlongWire = transObj->positionAlongWire();
  // added in 12.5
  persObj->m_timeOverThreshold = static_cast<float>(transObj->timeOverThreshold());

  const std::string trtCircleContName{"TRT_DriftCircles"};
  ElementLink<InDet::TRT_DriftCircleContainer>::index_type hashAndIndex{0};
  bool isFound{m_eventCnvTool->getHashAndIndex<InDet::TRT_DriftCircleContainer, InDet::TRT_DriftCircleOnTrack>(transObj, trtCircleContName, hashAndIndex)};
  persObj->m_prdLink.m_contName = (isFound ? trtCircleContName : "");
  persObj->m_prdLink.m_elementIndex = hashAndIndex;
}
