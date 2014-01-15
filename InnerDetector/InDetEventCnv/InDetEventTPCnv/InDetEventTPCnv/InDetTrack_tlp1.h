/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_TRACK_TLP1_H
#define INDET_TRACK_TLP1_H



//-----------------------------------------------------------------------------
// InDetRIO_OnTrack
//-----------------------------------------------------------------------------
#include "InDetEventTPCnv/InDetRIO_OnTrack/SiClusterOnTrack_p1.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/PixelClusterOnTrack_p1.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/SCT_ClusterOnTrack_p1.h"
#include "InDetEventTPCnv/InDetRIO_OnTrack/TRT_DriftCircleOnTrack_p1.h"

//-----------------------------------------------------------------------------
// InDetPrepRawData
//-----------------------------------------------------------------------------
#include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiCluster_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiWidth_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircle_p1.h"


// InDet cROTS

#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack_p1.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrack_p1.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrack_p1.h"

namespace InDet
{
   class Track_tlp1
   {
  public:
     Track_tlp1() {}

    // std::vector< InDet::PixelCluster_p1 >          m_pixelClusters;
    // std::vector< InDet::SiCluster_p1 >             m_siClusters;
    // std::vector< InDet::SiWidth_p1 >               m_siWidths;
    // std::vector< InDet::TRT_DriftCircle_p1 >       m_trtDriftCircles;

     std::vector< InDet::SiClusterOnTrack_p1 >                  m_siClustersOT;
     std::vector< InDet::PixelClusterOnTrack_p1 >               m_pixelClustersOT;
     std::vector< InDet::SCT_ClusterOnTrack_p1 >                m_sctClustersOT;
     std::vector< InDet::TRT_DriftCircleOnTrack_p1 >            m_trtDriftCirclesOT;
     std::vector< InDet::CompetingPixelClustersOnTrack_p1>      m_pixelCROTs;
     std::vector< InDet::CompetingSCT_ClustersOnTrack_p1>       m_sctCROTs;
     std::vector< InDet::CompetingTRT_DriftCirclesOnTrack_p1>   m_trtCROTs;
   };
}


#endif
