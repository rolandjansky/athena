/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigInDetEvent/TrigInDetTrack.h"
#undef private
#undef protected

#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p2.h"
#include "TrigInDetEventTPCnv/TrigInDetTrack_p2.h"

//-----------------------------------------------------------------------------
// Persistent to transient
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p2::persToTrans( const TrigInDetTrack_p2 *persObj,
			       TrigInDetTrack    *transObj,
			       MsgStream       &log )
{

  log << MSG::DEBUG << "TrigInDetTrackCnv_p2::persToTrans called " << endreq;

  transObj->m_algId       = persObj->m_algId      ;
  transObj->m_chi2        = persObj->m_chi2       ;
  transObj->m_NStrawHits  = persObj->m_NStrawHits ;
  transObj->m_NStraw      = persObj->m_NStraw     ;
  transObj->m_NStrawTime  = persObj->m_NStrawTime ;
  transObj->m_NTRHits     = persObj->m_NTRHits    ;  
  transObj->m_NPixelSpacePoints = persObj->m_NPixelSpacePoints;
  transObj->m_NSCT_SpacePoints = persObj->m_NSCT_SpacePoints;
  transObj->m_HitPattern = persObj->m_HitPattern;

  transObj->m_param       = createTransFromPStore( &m_fpCnv, persObj->m_param, log );
  transObj->m_endParam    = createTransFromPStore( &m_fpCnv, persObj->m_endParam, log );
  
}

//-----------------------------------------------------------------------------
// Transient to persistent
//-----------------------------------------------------------------------------
void TrigInDetTrackCnv_p2::transToPers( const TrigInDetTrack    *transObj,
			       TrigInDetTrack_p2 *persObj,
			       MsgStream       &log )
{

  log << MSG::DEBUG << "TrigInDetTrackCnv_p2::transToPers called " << endreq;

  persObj->m_algId       = transObj->m_algId      ;
  persObj->m_chi2        = transObj->m_chi2       ;
  persObj->m_NStrawHits  = transObj->m_NStrawHits ;
  persObj->m_NStraw      = transObj->m_NStraw     ;
  persObj->m_NStrawTime  = transObj->m_NStrawTime ;
  persObj->m_NTRHits     = transObj->m_NTRHits    ;  
  persObj->m_NPixelSpacePoints = transObj->m_NPixelSpacePoints;
  persObj->m_NSCT_SpacePoints = transObj->m_NSCT_SpacePoints;
  persObj->m_HitPattern = transObj->m_HitPattern;

  persObj->m_param       = toPersistent( &m_fpCnv, transObj->m_param, log );
  persObj->m_endParam    = toPersistent( &m_fpCnv, transObj->m_endParam, log );
  

}
