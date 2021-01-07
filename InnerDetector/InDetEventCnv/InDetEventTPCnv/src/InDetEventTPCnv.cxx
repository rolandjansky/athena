/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// generate the T/P converter entries
#include "AthenaKernel/TPCnvFactory.h"

#include "InDetEventTPCnv/InDetTrack_tlp1.h"
#include "InDetEventTPCnv/InDetTrack_tlp2.h"
#include "InDetEventTPCnv/PixelClusterContainer_tlp1.h"
#include "InDetEventTPCnv/PixelClusterContainerCnv_tlp1.h"
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
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaContainerCnv_tlp1.h"
#include "InDetEventTPCnv/InDetLowBetaInfo/InDetLowBetaCandidateCnv_tlp1.h"

using namespace InDet;
DECLARE_TPCNV_FACTORY(InDetLowBetaContainerCnv_tlp1,
                      InDet::InDetLowBetaContainer,
                      InDetLowBetaContainer_tlp1,
                      Athena::TPCnvVers::Current)
                      
DECLARE_TPCNV_FACTORY(InDetLowBetaCandidateCnv_tlp1,
                      InDet::InDetLowBetaCandidate,
                      InDetLowBetaCandidate_tlp1,
                      Athena::TPCnvVers::Current)
                      

DECLARE_TPCNV_FACTORY(PixelClusterContainerCnv_tlp1,
                      InDet::PixelClusterContainer,
                      InDet::PixelClusterContainer_tlp1,
                      Athena::TPCnvVers::Current)
                      
