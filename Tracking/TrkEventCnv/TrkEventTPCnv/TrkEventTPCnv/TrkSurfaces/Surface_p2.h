/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SURFACE_P2_TRK_H
#define SURFACE_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   Surface_p2.h
//
//-----------------------------------------------------------------------------

#include <vector>
#include "Identifier/Identifier.h"

namespace Trk
{
   class Surface_p2
   {
  public:
     Surface_p2() : m_associatedDetElementId(0), m_surfaceType(0) {}

     Identifier32::value_type               m_associatedDetElementId;
     std::vector<float>                     m_transform;
     uint8_t                                m_surfaceType; 
   };
}

#endif // SURFACE_p2_TRK_H
