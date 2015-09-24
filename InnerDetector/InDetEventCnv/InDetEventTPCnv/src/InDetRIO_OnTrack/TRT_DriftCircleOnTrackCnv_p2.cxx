/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircleOnTrackCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#undef private
#undef protected

#include "InDetEventTPCnv/InDetRIO_OnTrack/TRT_DriftCircleOnTrackCnv_p2.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"


void TRT_DriftCircleOnTrackCnv_p2::persToTrans( const InDet::TRT_DriftCircleOnTrack_p2 *persObj, InDet::TRT_DriftCircleOnTrack *transObj, MsgStream &log ) {
    //std::cout<<"READING TRT_DriftCircleOnTrackCnv_p2"<<std::endl;
    transObj->m_idDE      = persObj->m_idDE;// Needed by base class cnv, so do first
  
    // fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
    transObj->m_identifier = persObj->m_id;
    //transObj->m_localParams = createTransFromPStore( &m_localParCnv, persObj->m_localParams, log );
    //transObj->m_localErrMat = createTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, log );
    fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &transObj->m_localParams, log );
    // fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &transObj->m_localErrMat, log );
    Trk::ErrorMatrix dummy;
    fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
    EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "TRT_DriftCircleOnTrackCnv_p2");
    
    // m_globalPosition - transient?
    transObj->m_status    = static_cast<Trk::DriftCircleStatus>( persObj->m_status );
    transObj->m_highLevel = persObj->m_highLevel;
    // added in 12.5
    transObj->m_timeOverThreshold = (double) persObj->m_timeOverThreshold;
  
//new variables  
    transObj->m_localAngle        = persObj->m_localAngle;
    transObj->m_positionAlongWire = persObj->m_positionAlongWire;
   
//   m_elementLinkConverter.resetForCnv(persObj->m_elementLinkToIDCTRT_DriftCircleContainerNames);
//   m_elementLinkConverter.persToTrans(&persObj->m_elementLinkToIDCTRT_DriftCircleContainer,&transObj->m_rio,log);
    m_elCnv.persToTrans(&persObj->m_prdLink,&transObj->m_rio,log);  

    // Attempt to call supertool to fill in detElements
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
    persObj->m_id = transObj->m_identifier.get_identifier32().get_compact();
    persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->m_localParams, log );
    Trk::ErrorMatrix pMat;
    EigenHelpers::eigenMatrixToVector(pMat.values, transObj->m_localCovariance, "TRT_DriftCircleOnTrackCnv_p2");
    persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );   
    // m_globalPosition - transient?
    
    persObj->m_idDE      = transObj->m_idDE;
    persObj->m_status    = static_cast<unsigned int>( transObj->m_status );
    persObj->m_highLevel = transObj->m_highLevel;
   
//new variables  
    persObj->m_localAngle	       = transObj->m_localAngle;
    persObj->m_positionAlongWire = transObj->m_positionAlongWire;   
   
    // added in 12.5
    persObj->m_timeOverThreshold = (float) transObj->m_timeOverThreshold;
//   m_elementLinkConverter.resetForCnv(persObj->m_elementLinkToIDCTRT_DriftCircleContainerNames);
//   m_elementLinkConverter.transToPers(&transObj->m_rio,&persObj->m_elementLinkToIDCTRT_DriftCircleContainer,log);
    //m_elCnv.transToPers(&transObj->m_rio,&persObj->m_prdLink,log);  

    if (!transObj->m_rio.dataID().empty()) {
        persObj->m_prdLink.m_contName          = transObj->m_rio.dataID();// New suggestion from RD - above crashes
        persObj->m_prdLink.m_elementIndex     = transObj->m_rio.index();
    }else{
        persObj->m_prdLink.m_contName         = "";
        persObj->m_prdLink.m_elementIndex     = 0;
    }


}
