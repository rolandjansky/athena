/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamSpotID.h"

bool BeamSpot::ID::operator<( const ID & rhs) const { 
  if ( m_timeStamp < rhs.m_timeStamp ) return true;
  if ( m_timeStamp > rhs.m_timeStamp ) return false;
  if ( m_pileup < rhs.m_pileup ) return true;
  if ( m_pileup > rhs.m_pileup ) return false;
  if ( m_bcid < rhs.m_bcid ) return true;
  if ( m_bcid > rhs.m_bcid ) return false;
  if ( m_lumiBlock < rhs.m_lumiBlock ) return true;
  if ( m_lumiBlock > rhs.m_lumiBlock ) return false;
  if ( m_runNumber < rhs.m_runNumber ) return true;
  if ( m_runNumber > rhs.m_runNumber ) return false;
 return false;
}


