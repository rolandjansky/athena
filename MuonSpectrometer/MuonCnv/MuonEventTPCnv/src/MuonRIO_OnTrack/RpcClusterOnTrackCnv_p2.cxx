/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   RpcClusterOnTrackCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonRIO_OnTrack/RpcClusterOnTrackCnv_p2.h"


void RpcClusterOnTrackCnv_p2::
persToTrans( const Muon::RpcClusterOnTrack_p2 *persObj,
	     Muon::RpcClusterOnTrack *transObj, MsgStream &log )
{
   fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
   m_elCnv.persToTrans(&persObj->m_prdLink,&transObj->m_rio,log);  
   transObj->m_positionAlongStrip = persObj->m_positionAlongStrip;
   
}


void RpcClusterOnTrackCnv_p2::
transToPers( const Muon::RpcClusterOnTrack *,
	     Muon::RpcClusterOnTrack_p2 *, MsgStream & ) 
{
  throw std::runtime_error("RpcClusterOnTrackCnv_p2::transToPers is deprecated!"); 
}


