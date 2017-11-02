# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#---------------------------------------------------------
# jobOptions Fragment for InDet SR1 Cosmic reconstruction
# --------------------------------------------------------

# =============================================================================
# ---- preprocessing - PrepRawData and SpacePoint
# =============================================================================


theApp.Dlls += [ "InDetPrepRawDataFormation","SiClusterizationTool" ]
if DetFlags.makeRIO.SCT_on():
    include( "InDetPrepRawDataFormation/SCT_PrepRawDataFormation.py" )
if DetFlags.makeRIO.TRT_on():
    # TRT R(t) calibrations
    include ( "TRT_DriftFunctionTool/TRT_DriftFunctionTool_options.py" )
    if CosmicMonitorFlags.doSim:
        ToolSvc.TRT_DriftFunctionTool.TRT_Calibration = False
    else:
        print 'No calibration is yet available for real data'
        #include ( "TRT_ConditionsAlgs/TRT_StrawCalibrationTool_options.py" )
        #ToolSvc.TRT_DriftFunctionTool.CustomTRTCalibration = True

    theApp.Dlls += ["TRT_DriftCircleTool"]
    theApp.TopAlg += ["InDet::TRT_RIO_Maker/NewTRT_RIO_Maker"]
    NewTRT_RIO_Maker = Algorithm( "NewTRT_RIO_Maker" )
    NewTRT_RIO_Maker.TRTRIOLocation = "TRT_DiftCircles";

# ---- truth for PrepRawData
if CosmicMonitorFlags.doTruth:
    include ( "InDetTruthAlgs/InDetPrepRawDataTruth.py" )
# ---- space points
if DetFlags.makeRIO.SCT_on():
    include( "SiSpacePointFormation/SiTrackerTBSpacePointFinderOptions.py" )
    SiTrackerSpacePointFinder.ProcessPixels       = FALSE
    SiTrackerSpacePointFinder.xVertex = 0
    SiTrackerSpacePointFinder.yVertex = 9999999
    SiTrackerSpacePointFinder.zVertex = 0
    
# ---- truth for SpacePoints
    if CosmicMonitorFlags.doTruth:
        include ( "InDetTruthAlgs/InDetSpacePointTruth.py" )

if CosmicMonitorFlags.Refit or CosmicMonitorFlags.SiTBLineFitter:
    theApp.Dlls+=["TrkExTools"]
    include ('TrkExTools/BasicExtrapolatorCTB2004_StraightLine.py')
#
# --- not used as long as set TrackingVolumes internally off by default
# --- later set ToolSvc.CTBTrackingBuilder.CreateGeometryFromGeoModel=True
#   ToolSvc.ATLAS_PixelVolumeBuilder.TrackingMagFieldToolInstance="CTBMagFieldTool"
#   ToolSvc.ATLAS_SCT_VolumeBuilder.TrackingMagFieldToolInstance="CTBMagFieldTool"

# ---- RIO_OnTrackCreation (some fitters, all refitters)
if CosmicMonitorFlags.SiTBLineFitter or CosmicMonitorFlags.Refit:
    include( "TrkRIO_OnTrackCreator/RIO_OnTrackCreator_InDet.py")

if CosmicMonitorFlags.SiTBLineFitter:
    if DetFlags.makeRIO.SCT_on():
        include("SiTestBeamTracking/SiTestBeamTracking_options.py")
        SiTestBeamTracking.ExtrapolatorName=extrapolatorInstance
        SiTestBeamTracking.Cosmics=TRUE
        SiTestBeamTracking.tracksname = "SiCosmic_Tracks"
    ToolSvc.TBStraightLineFitter.Chi2cut_x=125
    ToolSvc.TBStraightLineFitter.Chi2cut_z=35
    ToolSvc.TBStraightLineFitter.ExtrapolatorName=extrapolatorInstance
    ToolSvc.TBStraightLineFitter.Cosmics=TRUE
    if DetFlags.makeRIO.TRT_on():
        include("InDetTestBeamTracking/InDetTestBeamTracking_options.py")
        InDetTestBeamTracking.TrackFitter="TBStraightLineFitter"
        InDetTestBeamTracking.ExtrapolatorName=extrapolatorInstance
        InDetTestBeamTracking.TRTHitsOnTrack=25
        InDetTestBeamTracking.Cosmics=TRUE
        InDetTestBeamTracking.InputTracksName = "SiCosmic_Tracks"
        InDetTestBeamTracking.OutputTracks = "InDetCosmic_Tracks"
        InDetTestBeamTracking.Chi2cut=100
        ToolSvc.TBStraightLineFitter.OutlierCut=10.
        if DetFlags.makeRIO.SCT_on():
            InDetTestBeamTracking.DriftCircleCut=0.9
            InDetTestBeamTracking.MaxDeltaPhi=0.005
            InDetTestBeamTracking.MaxOffset=0.5
        else:
            InDetTestBeamTracking.DriftCircleCut=2.0
            InDetTestBeamTracking.MaxDeltaPhi=0.025
            InDetTestBeamTracking.MaxOffset=15.

if CosmicMonitorFlags.SiTBLineFitter:
    include( "InDetAmbiguitySolver/InDetAmbiProc_jobOptions.py" )
    # set the type of tracks to save in AmbiguityProcessor
    InDetAmbiguitySolver = Algorithm( "InDetAmbiguitySolver" )
    InDetAmbiguitySolver.ResolvedTracksLocation="Tracks"
    if CosmicMonitorFlags.Refit:
        InDetAmbiguitySolver.TracksLocation = "ReFitted_Tracks"
    else:
        if not DetFlags.makeRIO.TRT_on():
            InDetAmbiguitySolver.TracksLocation = "SiCosmic_Tracks"
        else:
            InDetAmbiguitySolver.TracksLocation = "InDetCosmic_Tracks"

# ---- do we add truth info
if CosmicMonitorFlags.SiTBLineFitter:
    if CosmicMonitorFlags.doTruth:
        include( "InDetTruthAlgs/InDetTrackTruth.py" )

# --- run particle creation
if CosmicMonitorFlags.SiTBLineFitter:
    if CosmicMonitorFlags.ParticleCreation:
        include( "InDetParticleCreation/InDetParticleCreation_jobOptions.py" )

# ---- do we add truth info for the TrackParticle
if CosmicMonitorFlags.SiTBLineFitter:
    if CosmicMonitorFlags.doTruth and CosmicMonitorFlags.ParticleCreation:
        include( "InDetTruthAlgs/InDetTrackParticleTruth.py" )


