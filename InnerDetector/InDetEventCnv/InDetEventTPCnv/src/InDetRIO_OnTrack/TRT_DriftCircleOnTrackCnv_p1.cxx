/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircleOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/TRT_DriftCircleOnTrackCnv_p1.h"
//nclude "TrkEventTPCnv/helpers/CLHEPHelpers.h"


void TRT_DriftCircleOnTrackCnv_p1::
persToTrans( const InDet::TRT_DriftCircleOnTrack_p1 *persObj,
	     InDet::TRT_DriftCircleOnTrack *transObj, MsgStream &log ) 
{
  ElementLinkToIDCTRT_DriftCircleContainer rio;
  m_elCnv.persToTrans(&persObj->m_prdLink,&rio,log);  

  *transObj = InDet::TRT_DriftCircleOnTrack (rio,
                                             Trk::LocalParameters(),
                                             Amg::MatrixX(),
                                             persObj->m_idDE,
                                             Identifier(),
                                             persObj->m_positionAlongWire,
                                             persObj->m_localAngle,
                                             static_cast<Trk::DriftCircleStatus>( persObj->m_status ),
                                             persObj->m_highLevel,
                                             persObj->m_timeOverThreshold
                                             );

  fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
}


void TRT_DriftCircleOnTrackCnv_p1::
transToPers( const InDet::TRT_DriftCircleOnTrack    *transObj,
	     InDet::TRT_DriftCircleOnTrack_p1 *persObj, MsgStream &log) 
{
   persObj->m_RIO = baseToPersistent( &m_RIOCnv, transObj, log );
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
  persObj->m_prdLink.m_contName          = transObj->prepRawDataLink().dataID();// New suggestion from RD - above crashes
  persObj->m_prdLink.m_elementIndex     = transObj->prepRawDataLink().index();
}
