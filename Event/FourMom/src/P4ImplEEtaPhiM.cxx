/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/Lib/P4ImplEEtaPhiM.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////

P4ImplEEtaPhiM& P4ImplEEtaPhiM::operator=( const P4ImplEEtaPhiM& rhs ) 
{
  if ( this != &rhs ) {
    m_e   = rhs.m_e;
    m_eta = rhs.m_eta;
    m_phi = rhs.m_phi;
    m_m   = rhs.m_m;
  }
  return *this;
}
