/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#undef private
#undef protected

#include "InDetEventTPCnv/InDetRIO_OnTrack/SCT_ClusterOnTrackCnv_p2.h"
#include "Identifier/Identifier.h" 
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void SCT_ClusterOnTrackCnv_p2::persToTrans( const InDet::SCT_ClusterOnTrack_p2 *persObj, InDet::SCT_ClusterOnTrack *transObj, MsgStream &log ) {
    //std::cout<<"READING SCT_ClusterOnTrackCnv_p2"<<std::endl;

    //transObj->m_identifier = persObj->m_id;

#ifdef __IDENTIFIER_64BIT__
    // When reading in 32-bit id, must cast to unsigned int
    const Identifier::value_type upper = 0XFFFFFFFF00000000LL;
    const Identifier::value_type lower = 0X00000000FFFFFFFFLL;
    const Identifier::value_type testUpper = persObj->m_id & upper;
    const Identifier::value_type testLower = persObj->m_id & lower;
    if ( testUpper == 0 && testLower > 0) {
	Identifier32::value_type id1 = persObj->m_id;
        transObj->m_identifier = id1;
    }
    else transObj->m_identifier = persObj->m_id;
#else 
    transObj->m_identifier = persObj->m_id;
#endif /* __IDENTIFIER_64BIT__ */


    //transObj->m_localParams = createTransFromPStore( &m_localParCnv, persObj->m_localParams, log );
    //transObj->m_localErrMat = createTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, log );
    fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &transObj->m_localParams, log );
    // fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &transObj->m_localErrMat, log );
    Trk::ErrorMatrix dummy;
    fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
    EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "SCT_ClusterOnTrackCnv_p2");
    
    transObj->m_idDE = persObj->m_idDE;
    transObj->m_isbroad = persObj->m_isbroad;
    
    m_elCnv.persToTrans(&persObj->m_prdLink,&transObj->m_rio,log);  

    transObj->m_positionAlongStrip = persObj->m_positionAlongStrip;

    // Attempt to call supertool to fill in detElements
    m_eventCnvTool->recreateRIO_OnTrack(transObj);
    if (transObj->detectorElement()==0) 
        log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
            << "probably because of a problem with the Identifier/IdentifierHash : ("
            << transObj->identify()<<"/"<<transObj->idDE()<<endreq;
}

void SCT_ClusterOnTrackCnv_p2::transToPers( const InDet::SCT_ClusterOnTrack*transObj, InDet::SCT_ClusterOnTrack_p2 *persObj, MsgStream &log ) {

    persObj->m_id = transObj->m_identifier.get_compact();
    const Trk::RIO_OnTrack* tobj = static_cast<const Trk::RIO_OnTrack *>(transObj);

    m_eventCnvTool->prepareRIO_OnTrack(const_cast<Trk::RIO_OnTrack *>(tobj));
    persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->m_localParams, log );
    // persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &transObj->m_localErrMat, log );
    Trk::ErrorMatrix pMat;
    EigenHelpers::eigenMatrixToVector(pMat.values, transObj->m_localCovariance, "SCT_ClusterOnTrackCnv_p2");
    persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );
    
    
    persObj->m_idDE = transObj->m_idDE;
    persObj->m_isbroad = transObj->m_isbroad;
    
    persObj->m_positionAlongStrip = (float) transObj->m_positionAlongStrip;
    
    
    if (!transObj->m_rio.dataID().empty()) {
        persObj->m_prdLink.m_contName         = transObj->m_rio.dataID();
        persObj->m_prdLink.m_elementIndex     = transObj->m_rio.index();
    }else{
        persObj->m_prdLink.m_contName         = "";
        persObj->m_prdLink.m_elementIndex     = 0;
    }
}


