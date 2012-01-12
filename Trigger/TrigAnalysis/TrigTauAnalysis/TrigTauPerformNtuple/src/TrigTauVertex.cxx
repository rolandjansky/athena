/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauVertex.h"

ClassImp(TrigTauVertex)

#include <cmath>

TrigTauVertex::TrigTauVertex()
{
  reset();
}

void TrigTauVertex::reset()
{
  m_x = 0;
  m_y = 0;
  m_z = 0;
  
  m_chi2 = 0;
  m_nDoF = 0;
  
  m_nTracks = 0;
  m_type = 0;
  
  m_trackWeight.clear();
  
}

float TrigTauVertex::distance( const TrigTauVertex& other) const
{
  return std::sqrt( std::pow( m_x-other.m_x, 2) +
                    std::pow( m_y-other.m_y, 2) + 
                    std::pow( m_z-other.m_z, 2));
}
