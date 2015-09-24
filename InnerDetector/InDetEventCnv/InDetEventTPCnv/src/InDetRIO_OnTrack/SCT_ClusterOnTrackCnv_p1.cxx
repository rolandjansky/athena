/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SCT_ClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#undef private
#undef protected

#include "InDetEventTPCnv/InDetRIO_OnTrack/SCT_ClusterOnTrackCnv_p1.h"


void SCT_ClusterOnTrackCnv_p1::
persToTrans( const InDet::SCT_ClusterOnTrack_p1 *persObj,
	     InDet::SCT_ClusterOnTrack *transObj, MsgStream &log ) {
   fillTransFromPStore( &m_scCnv, persObj->m_RIO,  transObj, log );
//   m_elementLinkConverter.resetForCnv(persObj->m_elementLinkToIDCSCT_ClusterContainerNames);
//   m_elementLinkConverter.persToTrans(&persObj->m_elementLinkToIDCSCT_ClusterContainer,&transObj->m_rio,log);
   m_elCnv.persToTrans(&persObj->m_prdLink,&transObj->m_rio,log);  
   
   transObj->m_positionAlongStrip = persObj->m_positionAlongStrip;
}

void SCT_ClusterOnTrackCnv_p1::
transToPers( const InDet::SCT_ClusterOnTrack*transObj,
	     InDet::SCT_ClusterOnTrack_p1 *persObj, MsgStream &log )
{

   persObj->m_positionAlongStrip = (float) transObj->m_positionAlongStrip;
   persObj->m_RIO = baseToPersistent( &m_scCnv,  transObj, log );
   
// New suggestion from RD - as calling  m_elCnv.transToPers crashes
   if (transObj->m_rio.isValid() ) {
       persObj->m_prdLink.m_contName         = transObj->m_rio.dataID();
       persObj->m_prdLink.m_elementIndex     = transObj->m_rio.index();
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


