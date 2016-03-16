/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrackCnv_p1.h"

void MaterialEffectsOnTrackCnv_p1 :: persToTrans(
   const Trk::MaterialEffectsOnTrack_p1 *persObj,
   Trk::EstimatedBremOnTrack    *transObj,
   MsgStream& ) 
{
   *transObj = Trk::EstimatedBremOnTrack (persObj->m_tInX0,
                                          persObj->m_deltaP,
                                          0.0,
                                          persObj->m_sigmaQoverPSquared,
                                          transObj->associatedSurface(),
                                          Trk::undefinedDirection);
}

void MaterialEffectsOnTrackCnv_p1 :: transToPers(
   const Trk :: EstimatedBremOnTrack    *transObj,
   Trk :: MaterialEffectsOnTrack_p1 *persObj,
   MsgStream& ) 
{
   // not to be used anyway.
   persObj->m_tInX0              = transObj->thicknessInX0();
   persObj->m_deltaP             = transObj->retainedEnFraction();
   persObj->m_sigmaQoverPSquared = transObj->sigmaQoverPsquared();
}



