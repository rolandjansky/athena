/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include <cmath>

#include <typeinfo>
#include <cmath>

//#include "TrkSurfaces/Surface.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrackCnv_p2.h"

void MaterialEffectsOnTrackCnv_p2 :: persToTrans(
   const Trk::MaterialEffectsOnTrack_p2 *persObj,
   Trk::MaterialEffectsOnTrack          *transObj,
   MsgStream& log) 
{
  fillTransFromPStore(&m_mefBaseCnv, persObj->m_mefBase, transObj, log);
  if ((std::abs(persObj->m_deltaPhi) + std::abs(persObj->m_deltaTheta) >
       1.0e-10) ||
      (persObj->m_sigmaDeltaPhi + persObj->m_sigmaDeltaTheta > 1.0e-10)) {
    transObj->m_scatteringAngles =
      Trk::ScatteringAngles(persObj->m_deltaPhi,
                            persObj->m_deltaTheta,
                            persObj->m_sigmaDeltaPhi,
                            persObj->m_sigmaDeltaTheta);
  } else
    transObj->m_scatteringAngles = std::nullopt;
  transObj->m_energyLoss.reset(createTransFromPStore(
    (ITPConverterFor<Trk::EnergyLoss>**)nullptr, persObj->m_energyLoss, log));
}

void MaterialEffectsOnTrackCnv_p2 :: transToPers(
   const Trk::MaterialEffectsOnTrack *transObj,
   Trk::MaterialEffectsOnTrack_p2  *persObj,
   MsgStream& log) 
{
  persObj->m_mefBase = baseToPersistent( &m_mefBaseCnv, transObj, log );
  if (transObj->scatteringAngles()!=nullptr) {
    persObj->m_deltaPhi        = (float)transObj->scatteringAngles()->deltaPhi();
    persObj->m_deltaTheta      = (float)transObj->scatteringAngles()->deltaTheta();
    persObj->m_sigmaDeltaPhi   = (float)transObj->scatteringAngles()->sigmaDeltaPhi();
    persObj->m_sigmaDeltaTheta = (float)transObj->scatteringAngles()->sigmaDeltaTheta();
  }
  //  persObj->m_energyLoss = toPersistent( &m_elossCnv, transObj->m_energyLoss, log );  
//  if (transObj->m_energyLoss!=0) std::cout<<"Ending MaterialEffectsOnTrackCnv_p2::transToPers: "<<(typeid(*(transObj->m_energyLoss))).name()<<std::endl;
  persObj->m_energyLoss = toPersistent( (ITPConverterFor<Trk::EnergyLoss>**)nullptr, transObj->energyLoss(), log );
}



