/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETEVENTTPCNV_COMPETINGPIXELCLUSTERSONTRACK_P1_H
#define INDETEVENTTPCNV_COMPETINGPIXELCLUSTERSONTRACK_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>

namespace InDet
{
    /** Version 1 of the persistent class representing the transient class  InDet::CompetingPixelClustersOnTrack
    @author Edward.Moyse@cern.ch
    */
   class CompetingPixelClustersOnTrack_p1
   {
  public:
     CompetingPixelClustersOnTrack_p1() {}

     /** link to base CompetingRIOsOnTrack */
     TPObjRef                                       m_competingROT;

     std::vector< TPObjRef >                        m_containedChildRots;
   };
}

#endif 

