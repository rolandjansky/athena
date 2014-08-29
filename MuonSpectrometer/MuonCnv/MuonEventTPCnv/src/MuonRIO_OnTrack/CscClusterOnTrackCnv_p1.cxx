/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   CscClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#define private public
#define protected public
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#undef private
#undef protected

#include "MuonEventTPCnv/MuonRIO_OnTrack/CscClusterOnTrackCnv_p1.h"


void CscClusterOnTrackCnv_p1::
persToTrans( const Muon::CscClusterOnTrack_p1 *persObj,
	     Muon::CscClusterOnTrack *transObj, MsgStream &log )
{
   fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
   m_elCnv.persToTrans(&persObj->m_prdLink,&transObj->m_rio,log);  
   transObj->m_status=static_cast<Muon::CscClusterStatus>(persObj->m_status);
   transObj->m_positionAlongStrip = persObj->m_positionAlongStrip;
   
}


void CscClusterOnTrackCnv_p1::
transToPers( const Muon::CscClusterOnTrack * /**transObj*/,
	     Muon::CscClusterOnTrack_p1 * /**persObj*/, MsgStream & /**log*/ ) 
{
  throw std::runtime_error("CscClusterOnTrackCnv_p1::transToPers is deprecated!");    
}


