/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   RpcClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonEventTPCnv/MuonRIO_OnTrack/RpcClusterOnTrackCnv_p1.h"
#include "GeoPrimitives/GeoPrimitives.h"


void RpcClusterOnTrackCnv_p1::
persToTrans( const Muon::RpcClusterOnTrack_p1 *persObj,
	     Muon::RpcClusterOnTrack *transObj, MsgStream &log )
{
   fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
   m_elCnv.persToTrans(&persObj->m_prdLink,&transObj->m_rio,log);  
   transObj->m_positionAlongStrip = persObj->m_positionAlongStrip;

   // check to handle cases explained in bug#56227

   const MuonGM::RpcReadoutElement * de = transObj->detectorElement();
   if (de->Nphigasgaps() == 1 && de->NphiStripPanels() == 2){
      // okay, so this was originally on a large surface and needs correcting.
      const Amg::Vector3D& globalposHIT = de->stripPos( transObj->identify() );
      Amg::Vector2D tmpLocal;
      if (!de->surface( transObj->identify() ).globalToLocal(globalposHIT,globalposHIT,tmpLocal))
        throw std::runtime_error("RpcClusterOnTrackCnv_p1 - couldn't globalToLocal to surface");    
      
      Trk::DefinedParameter  xPar(tmpLocal[Trk::locX],Trk::locX);
      transObj->m_localParams = Trk::LocalParameters(xPar);
   }   
}


void RpcClusterOnTrackCnv_p1::
transToPers( const Muon::RpcClusterOnTrack *,
	     Muon::RpcClusterOnTrack_p1 *, MsgStream & ) 
{
  throw std::runtime_error("RpcClusterOnTrackCnv_p1::transToPers is deprecated!");    
}


