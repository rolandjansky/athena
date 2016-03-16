/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SCT_ClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/SCT_ClusterOnTrackCnv_p1.h"


void SCT_ClusterOnTrackCnv_p1::
persToTrans( const InDet::SCT_ClusterOnTrack_p1 *persObj,
	     InDet::SCT_ClusterOnTrack *transObj, MsgStream &log ) {
  
   ElementLinkToIDCSCT_ClusterContainer rio;
   m_elCnv.persToTrans(&persObj->m_prdLink,&rio,log);  

   Amg::MatrixX localCovariance;
   localCovariance.setZero();

   *transObj = InDet::SCT_ClusterOnTrack (rio,
                                          Trk::LocalParameters(),
                                          localCovariance,
                                          0,  // idDE
                                          Identifier(),
                                          false,
                                          persObj->m_positionAlongStrip);
   fillTransFromPStore( &m_scCnv, persObj->m_RIO,  transObj, log );
}

void SCT_ClusterOnTrackCnv_p1::
transToPers( const InDet::SCT_ClusterOnTrack*transObj,
	     InDet::SCT_ClusterOnTrack_p1 *persObj, MsgStream &log )
{

   persObj->m_positionAlongStrip = (float) transObj->positionAlongStrip();
   persObj->m_RIO = baseToPersistent( &m_scCnv,  transObj, log );
   
// New suggestion from RD - as calling  m_elCnv.transToPers crashes
   if (transObj->prepRawDataLink().isValid() ) {
     persObj->m_prdLink.m_contName         = transObj->prepRawDataLink().dataID();
     persObj->m_prdLink.m_elementIndex     = transObj->prepRawDataLink().index();
   }else{
       persObj->m_prdLink.m_contName         = "";
       persObj->m_prdLink.m_elementIndex     = 0;
// commented out because it spams the log files when reading AODs (tracks from electrons e.g. aso which are in AOD)
//       log<<MSG::WARNING<<"SCT_ClusterOnTrackCnv_p1::transToPers - ElementLink broken!"<<endreq;
//       if (transObj->prepRawData()!=0) 
//            log<<MSG::WARNING<<"Dumping PRD: "<<*(transObj->prepRawData())<<endreq;
//        else 
//            log<<MSG::WARNING<<"No PRD to dump (or indeed link to)!"<<endreq;
   }
}


