/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MaterialEffectsOnTrack_p1.h
// author: Lukasz Janyst <ljanyst@cern.ch>
//
//-----------------------------------------------------------------------------

#ifndef MATERIAL_EFFECTS_ON_TRACK_P1_TRK_H
#define MATERIAL_EFFECTS_ON_TRACK_P1_TRK_H

namespace Trk
{
   class MaterialEffectsOnTrack_p1
   {
  public:
     MaterialEffectsOnTrack_p1():m_tInX0{},m_deltaP{},m_sigmaQoverPSquared{} {}

     double m_tInX0;
     double m_deltaP;
     double m_sigmaQoverPSquared;
   };
}

#endif // MATERIAL_EFFECTS_ON_TRACK_P1_TRK_H
