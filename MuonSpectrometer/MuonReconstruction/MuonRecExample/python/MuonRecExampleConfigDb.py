# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.CfgGetter import addTool, addToolClone, addService, addAlgorithm, \
     addTypesToExcludeIfDefaultValue, addNamesToExcludeIfDefaultValue, addFullNamesToExcludeIfDefaultValue, \
     addPropertiesToExcludeIfDefault, \
     addTypesToSkipIfNotAvailable, addNamesToSkipIfNotAvailable, addFullNamesToSkipIfNotAvailable, \
     addTypesOnlyToSkip

from AthenaCommon.Constants import *  # FATAL,ERROR etc.

from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

addNamesToSkipIfNotAvailable( "MuonIsolationTool" )
addTypesOnlyToSkip( "ICaloNoiseTool" )

################################################################################
# Tools from other packages
################################################################################

# put this here until it is set in the appropriate package (needed for Trigger_topOptions_standalone.py", line 141)
addTool("TrkExTools.AtlasExtrapolator.AtlasExtrapolator","AtlasExtrapolator")

################################################################################
# Tools from MuonRecExample.MuonRecTools
################################################################################

addTool( "MuonRecExample.MuonRecTools.MuonClusterOnTrackCreator", "MuonClusterOnTrackCreator" )

addTool( "MuonRecExample.MuonRecTools.MdtDriftCircleOnTrackCreator", "MdtDriftCircleOnTrackCreator" )
addTool( "MuonRecExample.MuonRecTools.MdtTubeHitOnTrackCreator", "MdtTubeHitOnTrackCreator" )
addTool( "MuGirlNS::StauBetaTofTool","StauBetaTofTool")
addTool( "MuonRecExample.MuonRecTools.MdtDriftCircleOnTrackCreatorStau","MdtDriftCircleOnTrackCreatorStau")

addTool( "Muon::MuonPRDSelectionTool","MuonPRDSelectionTool")
addTool( "Muon::MuonPRDSelectionTool","MuonPRDSelectionToolStau",MdtDriftCircleOnTrackCreator="MdtDriftCircleOnTrackCreatorStau")

addTool( "MuonRecExample.MuonRecTools.AdjustableT0Tool", "AdjustableT0Tool" )

addTool( "MuonRecExample.MuonRecTools.MdtDriftCircleOnTrackCreatorAdjustableT0",      "MdtDriftCircleOnTrackCreatorAdjustableT0")

addTool( "MuonRecExample.MuonRecTools.MuonRotCreator", "MuonRotCreator" )

addTool( "MuonRecExample.MuonRecTools.MuonCombinePatternTool", "MuonCombinePatternTool" )
addTool( "MuonRecExample.MuonRecTools.MuonHoughPatternTool", "MuonHoughPatternTool" )
addTool( "MuonRecExample.MuonRecTools.MuonHoughPatternFinderTool", "MuonHoughPatternFinderTool" )

addService("MuonRecExample.MuonRecTools.AtlasTrackingGeometrySvc","AtlasTrackingGeometrySvc")

addTool( "MuonRecExample.MuonRecTools.MuonNavigator", "MuonNavigator" )

addTool( "Trk::MaterialEffectsUpdator", "MuonMaterialEffectsUpdator" )

addTool("MuonRecExample.MuonRecTools.MuonRK_Propagator","MuonRK_Propagator")
addTool("MuonRecExample.MuonRecTools.MuonSTEP_Propagator","MuonSTEP_Propagator")
addTool("MuonRecExample.MuonRecTools.MuonSTEP_Propagator","MuonPropagator")
addTool("MuonRecExample.MuonRecTools.MuonSTEP_Propagator","MCTBPropagator")
addTool("Trk::STEP_Propagator", "MuonStraightLinePropagator")

addTool("MuonRecExample.MuonRecTools.MuonExtrapolator", "MuonExtrapolator")
addTool("MuonRecExample.MuonRecTools.MuonStraightLineExtrapolator", "MuonStraightLineExtrapolator")

addTool("Trk::KalmanUpdator", "MuonMeasUpdator")

addService("Muon::MuonIdHelperSvc", "MuonIdHelperSvc", HasCSC=MuonGeometryFlags.hasCSC(), HasSTgc=MuonGeometryFlags.hasSTGC(), HasMM=MuonGeometryFlags.hasMM())

addTool("Muon::MuonTrackTruthTool", "MuonTrackTruthTool")

addTool("MuonRecExample.MooreTools.MuonTrackToSegmentTool", "MuonTrackToSegmentTool")

addService("MuonRecExample.MuonRecTools.MuonEDMHelperSvc", "MuonEDMHelperSvc")

addTool("MuonRecExample.MuonRecTools.MuonEDMPrinterTool", "MuonEDMPrinterTool")

addTool("MuonRecExample.MuonRecTools.MuonKalmanTrackFitter","MuonKalmanTrackFitter")

addTool("MuonRecExample.MuonRecTools.MuonTrackSummaryHelperTool","MuonTrackSummaryHelperTool")

addTool("MuonRecExample.MuonRecTools.MuonTrackSummaryTool","MuonTrackSummaryTool")

addTool("MuonRecExample.MuonRecTools.MuonParticleCreatorTool","MuonParticleCreatorTool")

addTool( "MuonRecExample.MuonRecTools.MuonChi2TrackFitter", "MuonChi2TrackFitter" )
addTool( "MuonRecExample.MuonRecTools.MuonChi2TrackFitter", "MuonChi2SLTrackFitter", StraightLine=True )

addTool( "MuonRecExample.MuonRecTools.MuonSegmentMomentum", "MuonSegmentMomentum" )
addTool( "MuonRecExample.MuonRecTools.MuonSegmentMomentumFromField", "MuonSegmentMomentumFromField")

addTool( "MuonRecExample.MuonRecTools.MuonPhiHitSelector", "MuonPhiHitSelector" )

addTool( "MuonRecExample.MuonRecTools.MdtSegmentT0Fitter", "MdtSegmentT0Fitter" )

addTool( "MuonRecExample.MuonRecTools.MdtMathSegmentFinder", "MdtMathSegmentFinder" )
addTool( "MuonRecExample.MuonRecTools.MdtMathT0FitSegmentFinder", "MdtMathT0FitSegmentFinder" )

addTool( "MuonRecExample.MuonRecTools.DCMathSegmentMaker", "DCMathSegmentMaker" )
addTool( "MuonRecExample.MuonRecTools.DCMathT0FitSegmentMaker", "DCMathT0FitSegmentMaker" )

addTool( "MuonRecExample.MuonRecTools.MuonClusterSegmentFinder", "MuonClusterSegmentFinder")
addTool( "MuonRecExample.MuonRecTools.MuonClusterSegmentFinderTool", "MuonClusterSegmentFinderTool" )
#N.B. Both of these are tools. They do slightly different things, but the naming is unfortunate.

addTool( "MuonRecExample.MuonRecTools.MuonLayerHoughTool","MuonLayerHoughTool" )

addTool( "Muon::MuonTruthSummaryTool","MuonTruthSummaryTool")

addTool( "Muon::MuonClusterizationTool","MuonClusterizationTool" )

# Set some Muon Properties in the global ResidualPullCalculator
# Those values should be in the C++ defaults really
addTool("Trk::ResidualPullCalculator","ResidualPullCalculator",
        ResidualPullCalculatorForRPC = "Muon::RPC_ResidualPullCalculator/RPC_ResidualPullCalculator",
        ResidualPullCalculatorForTGC = "Muon::TGC_ResidualPullCalculator/TGC_ResidualPullCalculator")


################################################################################
# Tools and related algorithms from MuonRecExample.MuonPrdProviderToolsConfig.py
################################################################################

addTool( "MuonRecExample.MuonPrdProviderToolsConfig.RpcPrepDataProviderTool", "RpcPrepDataProviderTool" )
addTool( "MuonRecExample.MuonPrdProviderToolsConfig.MdtPrepDataProviderTool", "MdtPrepDataProviderTool" )
addTool( "MuonRecExample.MuonPrdProviderToolsConfig.TgcPrepDataProviderTool", "TgcPrepDataProviderTool" )
if MuonGeometryFlags.hasCSC():
    addTool( "MuonRecExample.MuonPrdProviderToolsConfig.CscPrepDataProviderTool", "CscPrepDataProviderTool" )
    addAlgorithm("MuonRecExample.MuonPrdProviderToolsConfig.CscRdoToCscPrepData", "CscRdoToCscPrepData")
if (CommonGeometryFlags.Run() in ["RUN3", "RUN4"]):
    addTool( "MuonRecExample.MuonPrdProviderToolsConfig.MM_PrepDataProviderTool", "MM_PrepDataProviderTool" )
    addTool( "MuonRecExample.MuonPrdProviderToolsConfig.STGC_PrepDataProviderTool", "STGC_PrepDataProviderTool" )

################################################################################
# Tools from MuonRecExample.MooreTools
################################################################################


addTool("MuonRecExample.MooreTools.MuonSegmentSelectionTool","MuonSegmentSelectionTool")
addTool("MuonRecExample.MooreTools.MuonSegmentMatchingTool",'MuonSegmentMatchingTool')
addTool("MuonRecExample.MooreTools.MuonSegmentMatchingTool",'MuonSegmentMatchingToolTight',TightSegmentMatching=True)

# custom propagator and extrapolator

addTool("MuonRecExample.MooreTools.MCTBExtrapolator","MCTBExtrapolator")
addTool("MuonRecExample.MooreTools.MCTBExtrapolator","MCTBExtrapolatorBlendedMat", UseMuonMatApproximation = True )

addTool("MuonRecExample.MooreTools.MCTBFitter",  "MCTBFitter")
addTool("MuonRecExample.MooreTools.MCTBSLFitter","MCTBSLFitter")

addTool("MuonRecExample.MooreTools.MCTBFitter",   "MCTBFitterMaterialFromTrack", GetMaterialFromTrack=True)
addTool("MuonRecExample.MooreTools.MCTBSLFitterMaterialFromTrack", "MCTBSLFitterMaterialFromTrack")

addToolClone("MdtMathSegmentFinder", "MCTBMdtMathSegmentFinder", UseChamberTheta = False, AssociationRoadWidth = 1.5 )

addTool("MuonRecExample.MooreTools.MuonSeededSegmentFinder", "MuonSeededSegmentFinder")


addTool( "MuonRecExample.MooreTools.MuonRefitTool", "MuonRefitTool")


addTool("MuonRecExample.MooreTools.MuonErrorOptimisationTool","MuonErrorOptimisationTool")

addTool( "MuonRecExample.MooreTools.MuonTrackCleaner", "MuonTrackCleaner" )

addToolClone( "MuonClusterOnTrackCreator", "FixedErrorMuonClusterOnTrackCreator",
              DoFixedErrorCscEta = True, FixedErrorCscEta = .5 )

if MuonGeometryFlags.hasCSC():
    addTool( "MuonRecExample.MuonRecTools.CscClusterOnTrackCreator", "CscClusterOnTrackCreator"  )
    addTool( "MuonRecExample.MuonRecTools.CscBroadClusterOnTrackCreator", "CscBroadClusterOnTrackCreator" )

addTool( "MuonRecExample.MooreTools.MuonChamberHoleRecoveryTool", "MuonChamberHoleRecoveryTool", CscRotCreator=("Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator" if MuonGeometryFlags.hasCSC() else ""), CscPrepDataContainer=("CSC_Clusters" if MuonGeometryFlags.hasCSC() else ""))

addTool( "MuonRecExample.MooreTools.MuonSegmentRegionRecoveryTool", "MuonSegmentRegionRecoveryTool" )

addTool( "MuonRecExample.MooreTools.MuonTrackScoringTool", "MuonTrackScoringTool" )

addTool( "Muon::MuonAmbiTrackSelectionTool", "MuonAmbiSelectionTool" )

addTool( "MuonRecExample.MooreTools.MuonAmbiProcessor", "MuonAmbiProcessor" )

addTool( "MuonRecExample.MooreTools.MuonTrackSelectorTool", "MuonTrackSelectorTool" )

addTool("MuonRecExample.MooreTools.MuonTrackExtrapolationTool", "MuonTrackExtrapolationTool")

addToolClone("MuonSegmentRegionRecoveryTool","MuonEORecoveryTool",OnlyEO = True,
             Fitter="MCTBSLFitter", UseFitterOutlierLogic=False)

addTool( "MuonRecExample.MooreTools.MuonPatternSegmentMaker","MuonPatternSegmentMaker")

addTool("Muon::MuonPatternCalibration", "MuonPatternCalibration")

addTool( "MuonRecExample.MooreTools.MuonCurvedSegmentCombiner","MuonCurvedSegmentCombiner")

addTool( "MuonRecExample.MooreTools.MooSegmentCombinationFinder", "MooSegmentFinder" )

addToolClone( "MdtDriftCircleOnTrackCreator", "MdtDriftCircleOnTrackCreatorPreFit",
              DoFixedError = True, CreateTubeHit = True, DoSegmentErrors = False )


addTool( "MuonRecExample.MooreTools.MooCandidateMatchingTool","MooCandidateMatchingTool")

addTool( "MuonRecExample.MooreTools.MooTrackFitter", "MooTrackFitter")

addTool( "MuonRecExample.MooreTools.MooTrackFitter", "MooSLTrackFitter",
         Fitter="MCTBSLFitter",
         FitterPreFit="MCTBSLFitter",
         Propagator="MuonStraightLinePropagator",
         ReducedChi2Cut = 10.0 ,
         SLFit=True)

addTool( "MuonRecExample.MooreTools.MooTrackBuilder", "MooTrackBuilderTemplate")

if MuonGeometryFlags.hasCSC():
    addTool("MuonRecExample.CscTools.CscAlignmentTool","CscAlignmentTool")
    addTool("MuonRecExample.CscTools.CscClusterUtilTool","CscClusterUtilTool")
    addTool("MuonRecExample.CscTools.QratCscClusterFitter","QratCscClusterFitter")
    addTool("MuonRecExample.CscTools.CscPeakThresholdClusterBuilderTool","CscPeakThresholdClusterBuilderTool")
    addTool("MuonRecExample.CscTools.CscThresholdClusterBuilderTool","CscThresholdClusterBuilderTool")
    addTool("MuonRecExample.CscTools.CalibCscStripFitter","CalibCscStripFitter")
    addTool("MuonRecExample.CscTools.SimpleCscClusterFitter","SimpleCscClusterFitter")
    addTool("MuonRecExample.CscTools.CscSplitClusterFitter","CscSplitClusterFitter")

    addTool("MuonRecExample.CscTools.Csc2dSegmentMaker","Csc2dSegmentMaker")
    addTool("MuonRecExample.CscTools.Csc4dSegmentMaker","Csc4dSegmentMaker")
    addTool("MuonRecExample.CscTools.CscSegmentUtilTool","CscSegmentUtilTool")

    addAlgorithm("MuonRecExample.CscTools.CscThresholdClusterBuilder","CscThresholdClusterBuilder")

################################################################################
# Tools from MuonRecExample.NSWTools  (NSW - MicroMegas and STgc reconstruction tools)
################################################################################
addTool("MuonRecExample.NSWTools.SimpleMMClusterBuilderTool","SimpleMMClusterBuilderTool")
addTool("MuonRecExample.NSWTools.UTPCMMClusterBuilderTool","UTPCMMClusterBuilderTool")
addTool("MuonRecExample.NSWTools.ProjectionMMClusterBuilderTool","ProjectionMMClusterBuilderTool")
addTool("MuonRecExample.NSWTools.ConstraintAngleMMClusterBuilderTool","ConstraintAngleMMClusterBuilderTool")
addTool("MuonRecExample.NSWTools.ClusterTimeProjectionMMClusterBuilderTool","ClusterTimeProjectionMMClusterBuilderTool")
addTool("MuonRecExample.NSWTools.SimpleSTgcClusterBuilderTool","SimpleSTgcClusterBuilderTool")

addTool("NSWCalibTools.NSWCalibToolsConfig.MMCalibSmearingTool","MMCalibSmearingTool")
addTool("NSWCalibTools.NSWCalibToolsConfig.STgcCalibSmearingTool","STgcCalibSmearingTool")
addTool("NSWCalibTools.NSWCalibToolsConfig.NSWCalibTool","NSWCalibTool")

################################################################################
# Tools from MuonRecExample.MuPatTools
################################################################################

addTool( "MuonRecExample.MuPatTools.MuPatCandidateTool","MuPatCandidateTool", CscRotCreator=("Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator" if MuonGeometryFlags.hasCSC() else ""))

addTool( "MuonRecExample.MuPatTools.MuPatHitTool", "MuPatHitTool", CscRotCreator=("Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator" if MuonGeometryFlags.hasCSC() else ""))


################################################################################
# Tools from MuonRecExample.Moore
################################################################################

# combined segment cleaner
addTool("Muon::MuonSegmentCombinationCleanerTool","MuonSegmentCombinationCleanerTool")

################################################################################
# MuonStandalone
################################################################################

addTool( "MuonRecExample.MuonStandalone.MuonTrackSteering", "MuonTrackSteering" )
addTool("MuonRecExample.MuonRecTools.MuonSegmentFittingTool", "MuonSegmentFittingTool")
addTool("MuonRecExample.MuonRecTools.MuonLayerSegmentFinderTool", "MuonLayerSegmentFinderTool")

################################################################################
# MS vertex
################################################################################

addTool("Muon::MSVertexTrackletTool","MSVertexTrackletTool")
addTool("Muon::MSVertexRecoTool","MSVertexRecoTool")

################################################################################
# Alignment Error Tool
################################################################################

addTool("MuonAlign::AlignmentErrorTool","MuonAlignmentErrorTool")

