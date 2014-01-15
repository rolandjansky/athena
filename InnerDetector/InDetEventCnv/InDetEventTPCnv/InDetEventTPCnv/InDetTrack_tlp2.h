/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_TRACK_tlp2_H
#define INDET_TRACK_tlp2_H



//-----------------------------------------------------------------------------
// InDetRIO_OnTrack
//-----------------------------------------------------------------------------
#include "InDetEventTPCnv/InDetRIO_OnTrack/TRT_DriftCircleOnTrack_p2.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrack_p2.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/SCT_ClusterOnTrack_p2.h"


// InDet cROTS

#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack_p1.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrack_p1.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrack_p1.h"

namespace InDet
{
   class Track_tlp2
   {
  public:
     Track_tlp2() {}

     std::vector< InDet::PixelClusterOnTrack_p2 >               m_pixelClustersOT;
     std::vector< InDet::SCT_ClusterOnTrack_p2 >                m_sctClustersOT;
     std::vector< InDet::TRT_DriftCircleOnTrack_p2 >            m_trtDriftCirclesOT;
     std::vector< InDet::CompetingPixelClustersOnTrack_p1>      m_pixelCROTs;
     std::vector< InDet::CompetingSCT_ClustersOnTrack_p1>       m_sctCROTs;
     std::vector< InDet::CompetingTRT_DriftCirclesOnTrack_p1>   m_trtCROTs;
   };
}


#endif
