/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MaterialEffectsBase_p1.h
// author: Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
//
//-----------------------------------------------------------------------------

#ifndef MATERIAL_EFFECTS_BASE_P1_TRK_H
#define MATERIAL_EFFECTS_BASE_P1_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"

namespace Trk
{
   class MaterialEffectsBase_p1
   {
  public:
     MaterialEffectsBase_p1() : m_tInX0(0.0), m_typeFlags(0) {}

     float  m_tInX0;
     short  m_typeFlags;
     TPObjRef m_associatedSurface;
   };
}

#endif
