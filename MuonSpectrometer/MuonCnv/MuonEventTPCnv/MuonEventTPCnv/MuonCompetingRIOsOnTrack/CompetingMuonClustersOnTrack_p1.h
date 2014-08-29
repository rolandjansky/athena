/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTTPCNV_COMPETINGMUONCLUSTERSONTRACK_P1_H
#define MUONEVENTTPCNV_COMPETINGMUONCLUSTERSONTRACK_P1_H

//-----------------------------------------------------------------------------
//
// file:   CompetingMuonClustersOnTrack_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace Muon
{
    /** Version 1 of the persistent class representing the transient class Muon::MuonClusterOnTrack */
   class CompetingMuonClustersOnTrack_p1
   {
  public:
     CompetingMuonClustersOnTrack_p1() {}

     /** link to base MuonClusterOnTrack_p1, inheriting from RIO_OnTrack_p1 */
     TPObjRef                                       m_competingROT;

     std::vector< TPObjRef >                        m_containedChildRots;
     TPObjRef                                       m_associatedSurface;
     TPObjRef                                       m_parameters;
     TPObjRef                                       m_errorMatrix;
   };
}

#endif // CSC_CLUSTER_ON_TRACK_P1_TRK_H

