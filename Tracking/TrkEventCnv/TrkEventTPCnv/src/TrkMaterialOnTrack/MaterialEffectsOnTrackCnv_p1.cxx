/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#undef private

#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrackCnv_p1.h"

void MaterialEffectsOnTrackCnv_p1 :: persToTrans(
   const Trk::MaterialEffectsOnTrack_p1 *persObj,
   Trk::EstimatedBremOnTrack    *transObj,
   MsgStream& ) 
{
   transObj->m_tInX0              = persObj->m_tInX0;
   transObj->m_retainedEnFraction = persObj->m_deltaP;
   transObj->m_sigmaQoverPsquared = persObj->m_sigmaQoverPSquared;
    
   // log << MSG::INFO << "MEOT converter P->T" << endreq;

   transObj->m_bremSearchDirection = Trk::undefinedDirection;
   transObj->m_sigmaRetainedEnFraction = 0.0;
}

void MaterialEffectsOnTrackCnv_p1 :: transToPers(
   const Trk :: EstimatedBremOnTrack    *transObj,
   Trk :: MaterialEffectsOnTrack_p1 *persObj,
   MsgStream& ) 
{
   // not to be used anyway.
   persObj->m_tInX0              = transObj->m_tInX0;
   persObj->m_deltaP             = transObj->m_retainedEnFraction;
   persObj->m_sigmaQoverPSquared = transObj->m_sigmaQoverPsquared;
}



