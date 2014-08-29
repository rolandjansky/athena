/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MM_ClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonRIO_OnTrack/MM_ClusterOnTrackCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void MM_ClusterOnTrackCnv_p1::
persToTrans( const Muon::MM_ClusterOnTrack_p1 *persObj,
	     Muon::MMClusterOnTrack *transObj, MsgStream &log )
{
  // std::cout<<"BLAH! MM_ClusterOnTrackCnv_p1::persToTrans"<<std::endl;

  transObj->m_identifier = persObj->m_id;
  fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &transObj->m_localParams, log );
  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "RIO_OnTrackCnv_p2");
  
  m_elCnv.persToTrans(&persObj->m_prdLink,&transObj->m_rio,log);  
  transObj->m_positionAlongStrip = persObj->m_positionAlongStrip; 
  
   // Attempt to call supertool to fill in detElements
   m_eventCnvTool->recreateRIO_OnTrack(const_cast<Muon::MMClusterOnTrack *>(transObj));
   if (transObj->detectorElement()==0) 
        log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
            << "probably because of a problem with the Identifier/IdentifierHash : ("
            << transObj->identify()<<"/"<<transObj->idDE()<<endreq;   
}


void MM_ClusterOnTrackCnv_p1::
transToPers( const Muon::MMClusterOnTrack *transObj,
	     Muon::MM_ClusterOnTrack_p1 *persObj, MsgStream &log ) 
{
  // Prepare ELs
  // std::cout<<"BLAH! MM_ClusterOnTrackCnv_p1::transToPers"<<std::endl;
  
   m_eventCnvTool->prepareRIO_OnTrack(const_cast<Muon::MMClusterOnTrack *>(transObj));  
  
   m_elCnv.transToPers(&transObj->m_rio,&persObj->m_prdLink,log);
   persObj->m_positionAlongStrip = transObj->m_positionAlongStrip;
   persObj->m_id = transObj->m_identifier.get_identifier32().get_compact();
   persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->m_localParams, log );
   Trk::ErrorMatrix pMat;
   EigenHelpers::eigenMatrixToVector(pMat.values, transObj->m_localCovariance, "CscClusterOnTrackCnv_p2");
   persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );
}


