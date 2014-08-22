# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------
# jobOptions Fragment to fill CBNT ntuple 
# ---------------------------------------------

theApp.Dlls += [ "InDetTestBeamCBNT","InDetCabling" ]

# Fill RDO information
#CBNT_Athena.Members += [ "TestBeam::SCT_TB04_HitMaps/SCT_TB04_HitMaps"]
#SCT_TB04_HitMaps = Algorithm( "SCT_TB04_HitMaps" )
#SCT_TB04_HitMaps.DataObjectName = "SCT_RDOs"

CBNT_Athena.Members += [ "TestBeam::TRT_Fill_RDOs/TRT_Fill_RDOs" ]
TRT_Fill_RDOs = Algorithm( "TRT_Fill_RDOs" )
TRT_Fill_RDOs.DataObjectName = "TRT_RDOs"

if DetFlags.makeRIO.SCT_on():
    # Fill SCT Clusters 
    CBNT_Athena.Members += [ "TestBeam::FillSiCluster/FillSiCluster" ]
    FillSiCluster = Algorithm( "FillSiCluster" )
    FillSiCluster.SCT_SiClusterContainerName = "SCT_Clusters"
    FillSiCluster.Pixel_SiClusterContainerName = "PixelClusters"

    # Fill SCT Space Points
    CBNT_Athena.Members += [ "TestBeam::FillSpacePoint/FillSpacePoint" ]
    FillSpacePoint = Algorithm( "FillSpacePoint" )
    FillSpacePoint.SCT_SpacePointContainerName = "SCT_SpacePoints"
    FillSpacePoint.Pixel_SpacePointContainerName = "PixelSpacePoints"

if DetFlags.makeRIO.TRT_on():
    # Fill TRT Drift Circles
    CBNT_Athena.Members += [ "TestBeam::FillTRT_DriftCircles/FillTRT_DriftCircles" ]
    FillTRT_DriftCircles = Algorithm( "FillTRT_DriftCircles" )
    FillTRT_DriftCircles.TRT_DriftCircleContainerName = "TRT_DriftCircles"
   
if CosmicMonitorFlags.SiTBLineFitter:
    # Fill Track Information
    theApp.Dlls+=["TrkKalmanFitter"]
    CBNT_Athena.Members += [ "TestBeam::FillTrack/FillTrack" ]
    FillTrack = Algorithm( "FillTrack" )
    FillTrack.TrackName = "Tracks"
    FillTrack.getUnbiasedResiduals = False
    if CosmicMonitorFlags.doSim:
        FillTrack.getMC = True
    else:
        FillTrack.getMC = False


