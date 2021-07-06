/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETEVENTTPCNV_INDETEVENTTPCNVDICT_H
#define INDETEVENTTPCNV_INDETEVENTTPCNVDICT_H

//-----------------------------------------------------------------------------
//
// file:   InDetEventTPCnvDict_p1.h
//
//-----------------------------------------------------------------------------

#include "InDetEventTPCnv/InDetTrack_tlp1.h"
#include "InDetEventTPCnv/InDetTrack_tlp2.h"
#include "InDetEventTPCnv/PixelClusterContainer_tlp1.h"
#include "InDetEventTPCnv/InDetPrepRawData/SiCluster_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Collection_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/InDetPRD_Container_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p2.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelCluster_p3.h"
#include "InDetEventTPCnv/InDetPrepRawData/SCT_Cluster_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircle_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/TRT_DriftCircle_p2.h"
#include "InDetEventTPCnv/TRT_DriftCircleContainer_p2.h"
#include "InDetEventTPCnv/PixelClusterContainer_p2.h"
#include "InDetEventTPCnv/PixelClusterContainer_p3.h"
#include "InDetEventTPCnv/SCT_ClusterContainer_p2.h"
#include "InDetEventTPCnv/SCT_ClusterContainer_p3.h"
#include "InDetEventTPCnv/SCT_ClusterContainer_tlp1.h"
#include "InDetEventTPCnv/TRT_DriftCircleContainer_tlp1.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelGangedClusterAmbiguities_p1.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingPixelClustersOnTrack_p1.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingSCT_ClustersOnTrack_p1.h"
#include "InDetEventTPCnv/InDetCompetingRIOsOnTrack/CompetingTRT_DriftCirclesOnTrack_p1.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidate_p1.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaContainer_p1.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidate_tlp1.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaContainer_tlp1.h"

namespace InDetEventTPCnvDict {
 struct tmp
{
    std::vector< InDet::PixelCluster_p1>   		m_v1;
    std::vector< InDet::SCT_Cluster_p1>   		m_v2;
    std::vector< InDet::SCT_Cluster_p3>   		m_v2a;
    std::vector< InDet::SiCluster_p1>   		m_v3;
    std::vector< InDet::TRT_DriftCircle_p1 >   		m_v6;
    std::vector< InDet::TRT_DriftCircle_p2 >   		m_v6a;
    std::vector< InDet::InDetPRD_Collection_p1 >	m_v7;
    std::vector< InDet::InDetPRD_Container_p1 >		m_v8;
    std::vector< InDet::CompetingPixelClustersOnTrack_p1> m_v9;
    std::vector< InDet::CompetingSCT_ClustersOnTrack_p1> m_v10;
    std::vector< InDet::CompetingTRT_DriftCirclesOnTrack_p1> m_v11;
    std::vector< InDetLowBetaCandidate_p1> m_v12;
 };
} //> namespace InDetEventTPCnvDict

#endif // INDETEVENTTPCNV_INDETEVENTTPCNVDICT_H
