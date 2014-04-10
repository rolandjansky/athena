/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
\mainpage The InDetPrepRawData package 

\section introductionIDPRD Introduction

The InDetPrepRawData package provides InnerDetector specific implementations of the common TrkPrepRawData classes.

   - InDet::SiCluster - represents the ID silicon clusters. Extends the Trk::PrepRawData interface to provide:
      - SiWidth
   - InDet::PixelCluster (inherits from InDet::SiCluster) 
      - whether this cluster contains a ganged pixel
   - InDet::SCT_Cluster (inherits from InDet::SiCluster)
   - InDet::TRT_DriftCircle. Extends the Trk::PrepRawData interface to provide:
      - const bool highLevel(): whether the high level threshold was passed
      - const double timeOverThreshold(): time over threshold (ns)
      - const double driftTime(bool& isValid): the *corrected* drift time (ns) and a quality flag
      - const unsigned int getWord(): the raw dataword
      - int driftTimeBin(): the raw leading edge bin
      - const bool isNoise(): flag for high probability of noise origin
      - const InDetDD::TRT_BaseElement detectorElement(): detector element

There are also classes provided to define the various collections and containers.
   - PixelClusterCollection
   - SCT_ClusterCollection
   - SiClusterCollection
   - TRT_DriftCircleCollection
   - PixelClusterContainer
   - SCT_ClusterContainer
   - SiClusterContainer
   - TRT_DriftCircleContainer
   
*/
