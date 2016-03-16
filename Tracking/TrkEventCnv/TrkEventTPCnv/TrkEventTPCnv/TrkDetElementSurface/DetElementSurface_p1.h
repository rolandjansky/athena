/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETELEMENTSURFACE_P1_H
#define DETELEMENTSURFACE_P1_H

//-----------------------------------------------------------------------------
//
// file:   DetElementSurface_p1.h
//
//-----------------------------------------------------------------------------
#include "Identifier/Identifier.h"


namespace Trk
{
   class DetElementSurface_p1  {
  public:
     DetElementSurface_p1():m_id{} {}
     //Identifier::value_type m_id;
     Identifier32::value_type m_id;
   };
}

#endif 
