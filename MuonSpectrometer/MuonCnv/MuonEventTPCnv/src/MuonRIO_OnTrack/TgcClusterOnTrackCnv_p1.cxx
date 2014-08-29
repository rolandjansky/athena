/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TgcClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonRIO_OnTrack/TgcClusterOnTrackCnv_p1.h"


void TgcClusterOnTrackCnv_p1::
persToTrans( const Muon::TgcClusterOnTrack_p1 *persObj,
	     Muon::TgcClusterOnTrack *transObj, MsgStream &log )
{
   fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
   m_elCnv.persToTrans(&persObj->m_prdLink,&transObj->m_rio,log);  
   transObj->m_positionAlongStrip = persObj->m_positionAlongStrip;
   
}


void TgcClusterOnTrackCnv_p1::
transToPers( const Muon::TgcClusterOnTrack *,
	     Muon::TgcClusterOnTrack_p1 *, MsgStream & ) 
{
  throw std::runtime_error("TgcClusterOnTrackCnv_p1::transToPers is deprecated!"); 
}


