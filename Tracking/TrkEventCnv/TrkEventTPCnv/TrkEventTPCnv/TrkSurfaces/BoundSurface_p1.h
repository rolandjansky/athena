/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BOUND_SURFACE_P1_TRK_H
#define BOUND_SURFACE_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   BoundSurface_p1.h
//
//-----------------------------------------------------------------------------

#include "Surface_p1.h"

namespace Trk
{
   class BoundSurface_p1 : public Surface_p1
   {
  public:
     BoundSurface_p1() {}

     TPObjRef	m_bounds;
   };
}

#endif 
