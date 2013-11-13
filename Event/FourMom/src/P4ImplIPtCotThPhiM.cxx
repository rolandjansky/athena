/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/Lib/P4ImplIPtCotThPhiM.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////

P4ImplIPtCotThPhiM& 
P4ImplIPtCotThPhiM::operator=( const P4ImplIPtCotThPhiM& rhs ) 
{
  if ( this != &rhs ) {
    m_iPt   = rhs.m_iPt;
    m_cotTh = rhs.m_cotTh;
    m_phi   = rhs.m_phi;
    m_m     = rhs.m_m;
  }
  return *this;
}
