/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOUND_SURFACE_P2_TRK_H
#define BOUND_SURFACE_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   BoundSurface_p2.h
//
//-----------------------------------------------------------------------------

#include "Surface_p2.h"

namespace Trk
{
   class BoundSurface_p2 : public Surface_p2
   {
  public:
     BoundSurface_p2() {}

     TPObjRef	m_bounds;
   };
}

#endif 
