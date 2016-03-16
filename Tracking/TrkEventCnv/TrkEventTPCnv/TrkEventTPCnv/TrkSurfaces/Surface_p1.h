/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SURFACE_P1_TRK_H
#define SURFACE_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   Surface_p1.h
//
//-----------------------------------------------------------------------------

#include <vector>
#include "Identifier/Identifier.h"

namespace Trk
{
   class Surface_p1
   {
  public:
     Surface_p1() : m_associatedDetElementId(0) {}

//     unsigned			m_associatedDetElementId;
     //Identifier::value_type	m_associatedDetElementId;
     Identifier32::value_type	m_associatedDetElementId;
     #ifdef UseFloatsIn_Surface
     std::vector<float>    	m_transform;
     #else
     std::vector<double>    	m_transform;
     #endif
   };
}

#endif // SURFACE_P1_TRK_H
