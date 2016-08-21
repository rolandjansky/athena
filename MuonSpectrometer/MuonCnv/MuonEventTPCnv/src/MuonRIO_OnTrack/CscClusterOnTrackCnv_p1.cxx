/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   CscClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------


#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/CscClusterOnTrackCnv_p1.h"


void CscClusterOnTrackCnv_p1::
persToTrans( const Muon::CscClusterOnTrack_p1 *persObj,
	     Muon::CscClusterOnTrack *transObj, MsgStream &log )
{
  ElementLinkToIDC_CSC_Container rio;
  m_elCnv.persToTrans(&persObj->m_prdLink,&rio,log);  

  Amg::MatrixX locerr;
  locerr.setZero();
  *transObj = Muon::CscClusterOnTrack (rio,
                                       Trk::LocalParameters(), // locpos,
                                       locerr,
                                       Identifier(),
                                       nullptr, // detEL
                                       persObj->m_positionAlongStrip,
                                       static_cast<Muon::CscClusterStatus>(persObj->m_status));
   fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
}


void CscClusterOnTrackCnv_p1::
transToPers( const Muon::CscClusterOnTrack * /**transObj*/,
	     Muon::CscClusterOnTrack_p1 * /**persObj*/, MsgStream & /**log*/ ) 
{
  throw std::runtime_error("CscClusterOnTrackCnv_p1::transToPers is deprecated!");    
}


