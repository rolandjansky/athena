/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircleOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#undef private
#undef protected

#include "InDetEventTPCnv/InDetRIO_OnTrack/TRT_DriftCircleOnTrackCnv_p1.h"
//nclude "TrkEventTPCnv/helpers/CLHEPHelpers.h"


void TRT_DriftCircleOnTrackCnv_p1::
persToTrans( const InDet::TRT_DriftCircleOnTrack_p1 *persObj,
	     InDet::TRT_DriftCircleOnTrack *transObj, MsgStream &log ) 
{
  transObj->m_idDE      = persObj->m_idDE;// Needed by base class cnv, so do first
  fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
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

}


void TRT_DriftCircleOnTrackCnv_p1::
transToPers( const InDet::TRT_DriftCircleOnTrack    *transObj,
	     InDet::TRT_DriftCircleOnTrack_p1 *persObj, MsgStream &log) 
{
   persObj->m_RIO = baseToPersistent( &m_RIOCnv, transObj, log );
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
   persObj->m_prdLink.m_contName          = transObj->m_rio.dataID();// New suggestion from RD - above crashes
   persObj->m_prdLink.m_elementIndex     = transObj->m_rio.index();
}
