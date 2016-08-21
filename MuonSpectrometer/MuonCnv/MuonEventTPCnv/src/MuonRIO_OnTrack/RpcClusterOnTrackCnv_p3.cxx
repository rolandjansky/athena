/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   RpcClusterOnTrackCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/RpcClusterOnTrackCnv_p3.h"
#include "TrkEventTPCnv/helpers/EigenHelpers.h"


void RpcClusterOnTrackCnv_p3::
persToTrans( const Muon::RpcClusterOnTrack_p3 *persObj,
	     Muon::RpcClusterOnTrack *transObj, MsgStream &log )
{
  Trk::LocalParameters localParams;
  fillTransFromPStore( &m_localParCnv, persObj->m_localParams, &localParams, log );

  Trk::ErrorMatrix dummy;
  Amg::MatrixX localCovariance;
  fillTransFromPStore( &m_errorMxCnv, persObj->m_localErrMat, &dummy, log );
  EigenHelpers::vectorToEigenMatrix(dummy.values, localCovariance, "RpcClusterOnTrackCnv_p3");

  ElementLinkToIDC_RPC_Container rio;
  m_elCnv.persToTrans(&persObj->m_prdLink,&rio,log);  

  *transObj = Muon::RpcClusterOnTrack (rio,
                                       localParams,
                                       localCovariance,
                                       Identifier(persObj->m_id),
                                       nullptr, // detEL
                                       persObj->m_positionAlongStrip,
                                       persObj->m_time
                                       );

   // Attempt to call supertool to fill in detElements
   m_eventCnvTool->recreateRIO_OnTrack(const_cast<Muon::RpcClusterOnTrack *>(transObj));
   if (transObj->detectorElement()==0) 
        log << MSG::WARNING<<"Unable to reset DetEl for this RIO_OnTrack, "
            << "probably because of a problem with the Identifier/IdentifierHash : ("
            << transObj->identify()<<"/"<<transObj->idDE()<<endmsg;   
}


void RpcClusterOnTrackCnv_p3::
transToPers( const Muon::RpcClusterOnTrack *transObj,
	     Muon::RpcClusterOnTrack_p3 *persObj, MsgStream &log ) 
{
  // Prepare ELs
   m_eventCnvTool->prepareRIO_OnTrack(const_cast<Muon::RpcClusterOnTrack *>(transObj));  
  
   m_elCnv.transToPers(&transObj->prepRawDataLink(),&persObj->m_prdLink,log);
   persObj->m_positionAlongStrip = transObj->positionAlongStrip();
   persObj->m_time = transObj->time();
   persObj->m_id = transObj->identify().get_identifier32().get_compact();
   persObj->m_localParams = toPersistent( &m_localParCnv, &transObj->localParameters(), log );
   Trk::ErrorMatrix pMat;
   EigenHelpers::eigenMatrixToVector(pMat.values, transObj->localCovariance(), "RpcClusterOnTrackCnv_p3");
   persObj->m_localErrMat = toPersistent( &m_errorMxCnv, &pMat, log );
}


