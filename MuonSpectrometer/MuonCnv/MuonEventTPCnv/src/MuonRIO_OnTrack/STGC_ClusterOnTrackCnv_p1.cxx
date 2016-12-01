/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   STGCClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/sTgcClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/STGC_ClusterOnTrackCnv_p1.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"

void STGC_ClusterOnTrackCnv_p1::
persToTrans( const Muon::STGC_ClusterOnTrack_p1 *persObj,
	     Muon::sTgcClusterOnTrack *transObj, MsgStream &log )
{
  // std::cout<<"BLAH! STGC_ClusterOnTrackCnv_p1::persToTrans"<<std::endl;
  
  transObj->m_identifier = persObj->m_id;
  fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &transObj->m_localParams, log );
  Trk::ErrorMatrix dummy;
  fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, transObj->m_localCovariance, "RIO_OnTrackCnv_p2");
  
  m_elCnv.persToTrans(&persObj->m_prdLink,&transObj->m_rio,log);  
  transObj->m_positionAlongStrip = persObj->m_positionAlongStrip; 
  
   // Attempt to call supertool to fill in detElements
   m_eventCnvTool->recreateRIO_OnTrack(const_cast<Muon::sTgcClusterOnTrack *>(transObj));
   if (transObj->detectorElement()==0) 
        log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
            << "probably because of a problem with the Identifier/IdentifierHash : ("
            << transObj->identify()<<"/"<<transObj->idDE()<<")"<<endmsg;   
}


void STGC_ClusterOnTrackCnv_p1::
transToPers( const Muon::sTgcClusterOnTrack *transObj,
	     Muon::STGC_ClusterOnTrack_p1 *persObj, MsgStream &log ) 
{
  // std::cout<<"BLAH! STGC_ClusterOnTrackCnv_p1::persToTrans"<<std::endl;
  // log << MSG::INFO<<"Identifier/IdentifierHash : ("
  //     << transObj->identify()<<"/"<<transObj->idDE()<<endmsg;   
  
  // Prepare ELs
   m_eventCnvTool->prepareRIO_OnTrack(const_cast<Muon::sTgcClusterOnTrack *>(transObj));  
  
   m_elCnv.transToPers(&transObj->prepRawDataLink(),&persObj->m_prdLink,log);
   persObj->m_positionAlongStrip = transObj->positionAlongStrip();
   persObj->m_id = transObj->identify().get_identifier32().get_compact();
   persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->localParameters(), log );
   Trk::ErrorMatrix pMat;
   EigenHelpers::eigenMatrixToVector(pMat.values, transObj->localCovariance(), "STGC_ClusterOnTrackCnv_p1");
   persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );
  
   // Extra check.
   // Attempt to call supertool to fill in detElements
   Muon::sTgcClusterOnTrack * nonconst = const_cast<Muon::sTgcClusterOnTrack *>(transObj);
   nonconst->m_detEl=0;
   m_eventCnvTool->recreateRIO_OnTrack(const_cast<Muon::sTgcClusterOnTrack *>(nonconst));
   if (nonconst->detectorElement()==0) 
        log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
            << "probably because of a problem with the Identifier/IdentifierHash : ("
            << nonconst->identify()<<"/"<<nonconst->idDE()<<")"<<endmsg;   
}


