/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MM_ClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/MM_ClusterOnTrackCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void MM_ClusterOnTrackCnv_p1::
persToTrans( const Muon::MM_ClusterOnTrack_p1 *persObj,
	     Muon::MMClusterOnTrack *transObj, MsgStream &log )
{
   ElementLinkToIDC_MM_Container rio;
   m_elCnv.persToTrans(&persObj->m_prdLink,&rio,log);  

   Trk::LocalParameters localParams;
   fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &localParams, log );

   Trk::ErrorMatrix dummy;
   Amg::MatrixX localCovariance;
   fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
   EigenHelpers::vectorToEigenMatrix(dummy.values, localCovariance, "RIO_OnTrackCnv_p2");
   
   *transObj = Muon::MMClusterOnTrack (rio,
                                       localParams,
                                       localCovariance,
                                       Identifier (persObj->m_id),
                                       nullptr,
                                       persObj->m_positionAlongStrip
                                       );

   // Attempt to call supertool to fill in detElements
   m_eventCnvTool->recreateRIO_OnTrack(const_cast<Muon::MMClusterOnTrack *>(transObj));
   if (transObj->detectorElement()==0) 
        log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
            << "probably because of a problem with the Identifier/IdentifierHash : ("
            << transObj->identify()<<"/"<<transObj->idDE()<<endmsg;   
}


void MM_ClusterOnTrackCnv_p1::
transToPers( const Muon::MMClusterOnTrack *transObj,
	     Muon::MM_ClusterOnTrack_p1 *persObj, MsgStream &log ) 
{
  // Prepare ELs
  // std::cout<<"BLAH! MM_ClusterOnTrackCnv_p1::transToPers"<<std::endl;
  
   m_eventCnvTool->prepareRIO_OnTrack(const_cast<Muon::MMClusterOnTrack *>(transObj));  
  
   m_elCnv.transToPers(&transObj->prepRawDataLink(),&persObj->m_prdLink,log);
   persObj->m_positionAlongStrip = transObj->positionAlongStrip();
   persObj->m_id = transObj->identify().get_identifier32().get_compact();
   persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->localParameters(), log );
   Trk::ErrorMatrix pMat;
   EigenHelpers::eigenMatrixToVector(pMat.values, transObj->localCovariance(), "CscClusterOnTrackCnv_p2");
   persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );
}


