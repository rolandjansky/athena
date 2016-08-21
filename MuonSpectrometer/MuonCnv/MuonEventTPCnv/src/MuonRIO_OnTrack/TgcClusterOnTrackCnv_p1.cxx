/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TgcClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/TgcClusterOnTrackCnv_p1.h"


void TgcClusterOnTrackCnv_p1::
persToTrans( const Muon::TgcClusterOnTrack_p1 *persObj,
	     Muon::TgcClusterOnTrack *transObj, MsgStream &log )
{
  ElementLinkToIDC_TGC_Container rio;
  m_elCnv.persToTrans(&persObj->m_prdLink,&rio,log);  

  *transObj = Muon::TgcClusterOnTrack (rio,
                                       Trk::LocalParameters(),
                                       Amg::MatrixX(),
                                       Identifier(),
                                       nullptr,
                                       persObj->m_positionAlongStrip);
  fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
}


void TgcClusterOnTrackCnv_p1::
transToPers( const Muon::TgcClusterOnTrack *,
	     Muon::TgcClusterOnTrack_p1 *, MsgStream & ) 
{
  throw std::runtime_error("TgcClusterOnTrackCnv_p1::transToPers is deprecated!"); 
}


