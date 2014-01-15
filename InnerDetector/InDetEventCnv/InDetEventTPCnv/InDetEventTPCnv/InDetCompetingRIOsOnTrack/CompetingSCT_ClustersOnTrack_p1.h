/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETEVENTTPCNV_COMPETINGSCT_CLUSTERSONTRACK_P1_H
#define INDETEVENTTPCNV_COMPETINGSCT_CLUSTERSONTRACK_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace InDet
{
    /** Version 1 of the persistent class representing the transient class  InDet::CompetingSCT_ClustersOnTrack
    @author Edward.Moyse@cern.ch
    */
   class CompetingSCT_ClustersOnTrack_p1
   {
  public:
     CompetingSCT_ClustersOnTrack_p1() {}

     /** link to base CompetingRIOsOnTrack */
     TPObjRef                                       m_competingROT;

     std::vector< TPObjRef >                        m_containedChildRots;
   };
}

#endif 

