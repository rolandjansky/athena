/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   RpcClusterOnTrackCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/RpcClusterOnTrackCnv_p2.h"


void RpcClusterOnTrackCnv_p2::
persToTrans( const Muon::RpcClusterOnTrack_p2 *persObj,
	     Muon::RpcClusterOnTrack *transObj, MsgStream &log )
{
  ElementLinkToIDC_RPC_Container rio;
  m_elCnv.persToTrans(&persObj->m_prdLink,&rio,log);  

  *transObj = Muon::RpcClusterOnTrack (rio,
                                       Trk::LocalParameters(),
                                       Amg::MatrixX(),
                                       Identifier(),
                                       nullptr, // detEL
                                       persObj->m_positionAlongStrip,
                                       0 // time
                                       );
   
  fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
}


void RpcClusterOnTrackCnv_p2::
transToPers( const Muon::RpcClusterOnTrack *,
	     Muon::RpcClusterOnTrack_p2 *, MsgStream & ) 
{
  throw std::runtime_error("RpcClusterOnTrackCnv_p2::transToPers is deprecated!"); 
}


