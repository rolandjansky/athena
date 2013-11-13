/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/Lib/P4ImplPxPyPzE.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////

P4ImplPxPyPzE& P4ImplPxPyPzE::operator=( const P4ImplPxPyPzE& rhs ) 
{
  if ( this != &rhs ) {
    m_px = rhs.m_px;
    m_py = rhs.m_py;
    m_pz = rhs.m_pz;
    m_e  = rhs.m_e;
  }
  return *this;
}
