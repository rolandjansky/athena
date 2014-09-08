/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   ScatteringAngleOnTrackCnv_p1.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>, W Liebig
//
//-----------------------------------------------------------------------------

#define private public
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#undef private

#include "TrkEventTPCnv/TrkMaterialOnTrack/ScatteringAngleOnTrackCnv_p1.h"

void ScatteringAngleOnTrackCnv_p1::
persToTrans( const Trk::ScatteringAngleOnTrack_p1 *persObj,
             Trk::MaterialEffectsOnTrack    *transObj,
             MsgStream& )
{
  transObj->m_tInX0           = 0.0;
  transObj->m_scatteringAngles 
    = new Trk::ScatteringAngles(persObj->m_deltaPhi,persObj->m_deltaTheta,
                                persObj->m_sigmaDeltaPhi,persObj->m_sigmaDeltaTheta);
  transObj->m_energyLoss      = NULL;
  transObj->m_typeFlags       = 1;
  // FIXME: surface, deal with it here (free Sf) or later (detEl Sf)
}


void ScatteringAngleOnTrackCnv_p1::
transToPers( const Trk::MaterialEffectsOnTrack    *transObj,
             Trk::ScatteringAngleOnTrack_p1 *persObj,
             MsgStream& log)
{
  if (transObj->m_typeFlags != 1)
    log << MSG::ERROR << "wrong type, backwards "
        << "compatibility isn't possible." << endreq;
  if (transObj->m_scatteringAngles) {
    persObj->m_deltaPhi        = transObj->m_scatteringAngles->deltaPhi();
    persObj->m_deltaTheta      = transObj->m_scatteringAngles->deltaTheta();
    persObj->m_sigmaDeltaPhi   = transObj->m_scatteringAngles->sigmaDeltaPhi();
    persObj->m_sigmaDeltaTheta = transObj->m_scatteringAngles->sigmaDeltaTheta();
  } else {
    persObj->m_deltaPhi        = 0.0;
    persObj->m_deltaTheta      = 0.0;
    persObj->m_sigmaDeltaPhi   = 0.0;
    persObj->m_sigmaDeltaTheta = 0.0;
  }
}
