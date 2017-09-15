/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetCompetingRIOsOnTrackTool_page The InDetCompetingRIOsOnTrackTool package
The InDet package for CompetingRIOsOnTrack creation.

@author Sebastian.Fleischmann@cern.ch

@section InDetCompetingRIOsOnTrackTool_introductionInDetCompetingRIOsOnTrackTool Introduction

This package contains the tools to create Trk::CompetingRIOsOnTrack for the InDet realm
(i.e. InDet::CompetingTRT_DriftCirclesOnTrack, InDet::CompetingPixelClustersOnTrack, InDet::CompetingSCT_ClustersOnTrack). This includes
    creation of the RIO_OnTrack objects with the corresponding tools.

    From each TRT_DriftCircle (RIO) two TRT_DriftCircleOnTrack (ROTs) are created. This
    corresponds to the left and the right solution of the DriftCircle.

@section InDetCompetingRIOsOnTrackTool_InDetCompetingRIOsOnTrackToolOverview Class Overview
  The InDetCompetingRIOsOnTrackTool package contains the following classes:

  - InDet::ICompetingPixelClustersOnTrackCreator : Interface class for CompetingPixelClustersOnTrackTools.
  - InDet::ICompetingSCT_ClustersOnTrackCreator : Interface class for CompetingSCT_ClustersOnTrackTools.
  - InDet::ICompetingTRT_DriftCirclesOnTrackCreator : Interface class for CompetingTRT_DriftCirclesOnTrackTools.
  - InDet::CompetingPixelClustersOnTrackTool : creates InDet::CompetingPixelClustersOnTrack objects using a given
    track hypothesis and a list of InDet::PixelCluster (given as Trk::PrepRawData). It creates
    the InDet::PixelClusterOnTrack objects with the PixelClusterOnTrackTool. All InDet::PixelCluster have to
    belong to the same detector element.
  - InDet::CompetingSCT_ClustersOnTrackTool : creates InDet::CompetingSCT_ClustersOnTrack objects using a given
    track hypothesis and a list of InDet::SCT_Cluster (given as Trk::PrepRawData). It creates
    the InDet::SCT_ClusterOnTrack objects with the SCT_ClusterOnTrackTool.  All InDet::SCT_Cluster have to
    belong to the same detector element.
  - InDet::CompetingTRT_DriftCirclesOnTrackTool : creates InDet::CompetingTRT_DriftCirclesOnTrack objects using a given
    track hypothesis and a list of InDet::TRT_DriftCircle (given as Trk::PrepRawData). It creates
    the InDet::TRT_DriftCircleOnTrack objects with the TRT_DriftCircleOnTrackTool.
    From each TRT_DriftCircle (RIO) two TRT_DriftCircleOnTrack (ROTs) are created. This
    corresponds to the left and the right solution of the DriftCircle. The TRT_DriftCircle may belong to different
    straw tubes and the effective measurement is projected to the tube of the most probable measurement.

@section InDetCompetingRIOsOnTrackTool_ExtrasInDetCompetingRIOsOnTrackTool Extra Pages

*/
