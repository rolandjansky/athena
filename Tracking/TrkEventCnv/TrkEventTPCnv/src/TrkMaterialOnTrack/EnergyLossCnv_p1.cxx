/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLossCnv_p1.h"

void EnergyLossCnv_p1 :: persToTrans(
   const Trk::EnergyLoss_p1 *persObj,
   Trk :: EnergyLoss    *transObj,
   MsgStream& ) 
{
  *transObj = Trk::EnergyLoss (persObj->m_deltaE,
                               persObj->m_sigmaDeltaE,
                               persObj->m_sigmaMinusDeltaE,
                               persObj->m_sigmaPlusDeltaE,
                               0, 0, 0, 0,
                               // Questionable, but preserves previous behavior.
                               transObj->length());
}

void EnergyLossCnv_p1 :: transToPers(
   const Trk::EnergyLoss    *transObj,
   Trk::EnergyLoss_p1 *persObj,
   MsgStream& ) 
{
  persObj->m_deltaE              = (float)transObj->deltaE();
  persObj->m_sigmaDeltaE         = (float)transObj->sigmaDeltaE();
  persObj->m_sigmaMinusDeltaE    = (float)transObj->sigmaMinusDeltaE();
  persObj->m_sigmaPlusDeltaE     = (float)transObj->sigmaPlusDeltaE();
}



