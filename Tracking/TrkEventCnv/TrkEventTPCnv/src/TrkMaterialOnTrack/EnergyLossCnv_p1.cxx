/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrkMaterialOnTrack/EnergyLoss.h"
#undef private

#include "TrkEventTPCnv/TrkMaterialOnTrack/EnergyLossCnv_p1.h"

void EnergyLossCnv_p1 :: persToTrans(
   const Trk::EnergyLoss_p1 *persObj,
   Trk :: EnergyLoss    *transObj,
   MsgStream& ) 
{
  transObj->m_deltaE              = persObj->m_deltaE;
  transObj->m_sigmaDeltaE         = persObj->m_sigmaDeltaE;
  transObj->m_sigmaMinusDeltaE    = persObj->m_sigmaMinusDeltaE;
  transObj->m_sigmaPlusDeltaE     = persObj->m_sigmaPlusDeltaE;
}

void EnergyLossCnv_p1 :: transToPers(
   const Trk::EnergyLoss    *transObj,
   Trk::EnergyLoss_p1 *persObj,
   MsgStream& ) 
{
  persObj->m_deltaE              = (float)transObj->m_deltaE;
  persObj->m_sigmaDeltaE         = (float)transObj->m_sigmaDeltaE;
  persObj->m_sigmaMinusDeltaE    = (float)transObj->m_sigmaMinusDeltaE;
  persObj->m_sigmaPlusDeltaE     = (float)transObj->m_sigmaPlusDeltaE;
}



