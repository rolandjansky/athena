/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BeamBackgroundData.cxx 589193 2014-03-25 00:13:52Z tsulaia $
#include "RecBackgroundEvent/BeamBackgroundData.h"


//------------------------------------------------------------------------------
/**
 * This function looks for the muon segment among the beam halo muon candidates
 */
int BeamBackgroundData::GetIndexSeg(const Trk::Segment* seg) const
{
  int index = -1;

  for(int i=0; i<m_numSegment; i++) {
    if(seg == GetIndexSeg(i)) {
      index = i;
      break;
    }
  }

  return index;
}


//------------------------------------------------------------------------------
/**
 * This function looks for the calorimeter cluster among the beam halo muon candidates
 */
int BeamBackgroundData::GetIndexClus(const xAOD::CaloCluster* clus) const
{
  int index = -1;

  for(int i=0; i<m_numMatched; i++) {
    if(clus == GetIndexClus(i)) {
      index = i;
      break;
    }
  }

  return index;
}


//------------------------------------------------------------------------------
/**
 * This function looks for the jet among the fake jets candidates
 */
int BeamBackgroundData::GetIndexJet(const xAOD::Jet* jet) const
{
  int index = -1;

  for(int i=0; i<m_numJet; i++) {
    if(jet == GetIndexJet(i)) {
      index = i;
      break;
    }
  }

  return index;
}
