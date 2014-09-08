/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MaterialEffectsOnTrack_p2.h
// author: Wolfgang Liebig <lhttp://consult.cern.ch/xwho/people/54608>
//
//-----------------------------------------------------------------------------

#ifndef MATERIAL_EFFECTS_ON_TRACK_P2_TRK_H
#define MATERIAL_EFFECTS_ON_TRACK_P2_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
  /** @brief persistent representation v2 of MaterialEffectsOnTrack
      @author: Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
  */

   class MaterialEffectsOnTrack_p2
   {
  public:
     MaterialEffectsOnTrack_p2() :m_deltaPhi(0.0), m_deltaTheta(0.0), m_sigmaDeltaPhi(0.0), m_sigmaDeltaTheta(0.0) {}

     TPObjRef m_mefBase; // MaterialEffectsBase.h base class

     // TPObjRef has overhead, so just take the doubles.
     float m_deltaPhi;
     float m_deltaTheta;
     float m_sigmaDeltaPhi;
     float m_sigmaDeltaTheta;

     TPObjRef m_energyLoss;       // was: const EnergyLoss*
   };
}

#endif
