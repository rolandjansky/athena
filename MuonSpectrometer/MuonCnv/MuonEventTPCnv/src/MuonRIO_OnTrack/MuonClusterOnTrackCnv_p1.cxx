/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MuonClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include "MuonEventTPCnv/MuonRIO_OnTrack/MuonClusterOnTrackCnv_p1.h"


void MuonClusterOnTrackCnv_p1::
persToTrans( const Muon::MuonClusterOnTrack_p1 *persObj,
	     Muon::MuonClusterOnTrack *transObj, MsgStream &log )
{
   //std::cout<<"MuonClusterOnTrackCnv_p1::persToTrans start "<<std::endl;
   fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
   transObj->m_positionAlongStrip = persObj->m_positionAlongStrip;
   //std::cout<<"MuonClusterOnTrackCnv_p1::persToTrans end"<<std::endl;
}


void MuonClusterOnTrackCnv_p1::
transToPers( const Muon::MuonClusterOnTrack *transObj,
	     Muon::MuonClusterOnTrack_p1 *persObj, MsgStream &log ) 
{
   //std::cout<<"MuonClusterOnTrackCnv_p1::transToPers start"<<std::endl;
   persObj->m_RIO = baseToPersistent( &m_RIOCnv,  static_cast<const Trk::RIO_OnTrack*>(transObj), log );
   persObj->m_positionAlongStrip = transObj->positionAlongStrip();
   //std::cout<<"MuonClusterOnTrackCnv_p1::transToPers end"<<std::endl;
}


