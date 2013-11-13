/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FourMom/Lib/P4ImplPtEtaPhiM.h"

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////

// Constructors
////////////////

P4ImplPtEtaPhiM& P4ImplPtEtaPhiM::operator=( const P4ImplPtEtaPhiM& rhs ) 
{
  if ( this != &rhs ) {
    m_pt  = rhs.m_pt;
    m_eta = rhs.m_eta;
    m_phi = rhs.m_phi;
    m_m   = rhs.m_m;
  }
  return *this;
}
