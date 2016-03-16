/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETEVENTTPCNV_COMPETINGTRT_DRIFTCIRCLESONTRACK_P1_H
#define INDETEVENTTPCNV_COMPETINGTRT_DRIFTCIRCLESONTRACK_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace InDet
{
    /** Version 1 of the persistent class representing the transient class  InDet::CompetingTRT_DriftCirclesOnTrack
    @author Edward.Moyse@cern.ch
    */
   class CompetingTRT_DriftCirclesOnTrack_p1
   {
  public:
     CompetingTRT_DriftCirclesOnTrack_p1() {}

     /** link to base CompetingRIOsOnTrack */
     TPObjRef                                       m_competingROT;

     std::vector< TPObjRef >                        m_containedChildRots;
     TPObjRef                                       m_associatedSurface;
   };
}

#endif 

