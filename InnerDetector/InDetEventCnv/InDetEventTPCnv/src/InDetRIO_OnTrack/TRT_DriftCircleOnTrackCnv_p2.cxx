/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
            << transObj->identify()<<"/"<<transObj->idDE()<<endreq;

}


void TRT_DriftCircleOnTrackCnv_p2::transToPers( const InDet::TRT_DriftCircleOnTrack    *transObj, InDet::TRT_DriftCircleOnTrack_p2 *persObj, MsgStream &log) {
    //std::cout<<"WRITING TRT_DriftCircleOnTrackCnv_p2"<<std::endl;
    const Trk::RIO_OnTrack* tobj = static_cast<const Trk::RIO_OnTrack *>(transObj);
    m_eventCnvTool->prepareRIO_OnTrack(const_cast<Trk::RIO_OnTrack *>(tobj));
    // persObj->m_RIO = baseToPersistent( &m_RIOCnv, transObj, log );
    persObj->m_id = transObj->identify().get_identifier32().get_compact();
    persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->localParameters(), log );
    Trk::ErrorMatrix pMat;
    EigenHelpers::eigenMatrixToVector(pMat.values, transObj->localCovariance(), "TRT_DriftCircleOnTrackCnv_p2");
    persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );   
    // m_globalPosition - transient?
    
    persObj->m_idDE      = transObj->idDE();
    persObj->m_status    = static_cast<unsigned int>( transObj->status() );
    persObj->m_highLevel = transObj->highLevel();
   
//new variables  
    persObj->m_localAngle	       = transObj->localAngle();
    persObj->m_positionAlongWire = transObj->positionAlongWire();   
   
    // added in 12.5
    persObj->m_timeOverThreshold = (float) transObj->timeOverThreshold();
//   m_elementLinkConverter.resetForCnv(persObj->m_elementLinkToIDCTRT_DriftCircleContainerNames);
//   m_elementLinkConverter.transToPers(&transObj->m_rio,&persObj->m_elementLinkToIDCTRT_DriftCircleContainer,log);
    //m_elCnv.transToPers(&transObj->m_rio,&persObj->m_prdLink,log);  

    if (!transObj->prepRawDataLink().dataID().empty()) {
      persObj->m_prdLink.m_contName          = transObj->prepRawDataLink().dataID();// New suggestion from RD - above crashes
      persObj->m_prdLink.m_elementIndex     = transObj->prepRawDataLink().index();
    }else{
        persObj->m_prdLink.m_contName         = "";
        persObj->m_prdLink.m_elementIndex     = 0;
    }


}
