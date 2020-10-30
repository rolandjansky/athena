# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# ------------------------------------------------------------
#
# ----------- TRT Data-Preparation stage
#
# ------------------------------------------------------------
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory

def InDetPRD_MultiTruthMakerTRTCfg(flags, name = "InDetTRT_PRD_MultiTruthMaker", **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("PixelClusterContainerName", "")
    kwargs.setdefault("SCTClusterContainerName", "")
    kwargs.setdefault("TRTDriftCircleContainerName", 'TRT_DriftCircles') ##read from InDetKeys.TRT_DriftCircles
    kwargs.setdefault("SimDataMapNamePixel", "")
    kwargs.setdefault("SimDataMapNameSCT", "")
    kwargs.setdefault("SimDataMapNameTRT", 'TRT_SDO_Map') ##read from InDetKeys.TRT_SDOs
    kwargs.setdefault("TruthNamePixel", "")
    kwargs.setdefault("TruthNameSCT", "")
    kwargs.setdefault("TruthNameTRT", 'PRD_MultiTruthTRT') ##read from InDetKeys.TRT_DriftCirclesTruth

    acc.addEventAlgo(CompFactory.InDet.PRD_MultiTruthMaker(name, **kwargs))
    return acc

def InDetPRD_MultiTruthMakerTRTPUCfg(flags, name = "InDetTRT_PRD_MultiTruthMakerPU", **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("PixelClusterContainerName", "")
    kwargs.setdefault("SCTClusterContainerName", "")
    kwargs.setdefault("TRTDriftCircleContainerName", 'TRT_PU_DriftCircles') ##read from InDetKeys.TRT_PU_DriftCircles
    kwargs.setdefault("SimDataMapNamePixel", "")
    kwargs.setdefault("SimDataMapNameSCT", "")
    kwargs.setdefault("SimDataMapNameTRT", 'TRT_PU_SDO_Map') ##read from InDetKeys.TRT_PU_SDOs
    kwargs.setdefault("TruthNamePixel", "")
    kwargs.setdefault("TruthNameSCT", "")
    kwargs.setdefault("TruthNameTRT", 'PRD_PU_MultiTruthTRT') ##read from InDetKeys.TRT_PU_DriftCirclesTruth

    acc.addEventAlgo(CompFactory.InDet.PRD_MultiTruthMaker(name, **kwargs))
    return acc

def InDetTRT_DriftFunctionToolCfg(flags, useTimeInfo, usePhase, name = "InDetTRT_DriftFunctionTool", **kwargs):
    acc = ComponentAccumulator()
    #
    # --- TRT_DriftFunctionTool
    #
    # Calibration DB Service
    from InDetOverlay.TRT_ConditionsConfig import TRT_CalDbToolCfg
    InDetTRTCalDbTool = acc.popToolsAndMerge(TRT_CalDbToolCfg(flags))
    acc.addPublicTool(InDetTRTCalDbTool)

    # --- overwrite for uncalibrated DC production
    if (not useTimeInfo) or flags.InDet.noTRTTiming:
        kwargs.setdefault("DummyMode", True)
        kwargs.setdefault("UniversalError", 1.15)
    
    # --- set Data/MC flag
    if( flags.Input.isMC ) :
        kwargs.setdefault("IsMC", True)
    else :
        kwargs.setdefault("IsMC", False)

    # --- overwrite for calibration of MC
    if usePhase and flags.Beam.Type =='cosmics' and flags.Input.isMC:
        kwargs.setdefault("AllowDigiVersionOverride", True)
        kwargs.setdefault("ForcedDigiVersion", 9)

    kwargs.setdefault("TRTCalDbTool", InDetTRTCalDbTool)
    # --- set HT corrections
    kwargs.setdefault("HTCorrectionBarrelXe", 1.5205)
    kwargs.setdefault("HTCorrectionEndcapXe", 1.2712)
    kwargs.setdefault("HTCorrectionBarrelAr", 1.5205)
    kwargs.setdefault("HTCorrectionEndcapAr", 1.2712)
    # --- set ToT corrections
    kwargs.setdefault("ToTCorrectionsBarrelXe"       , [0., 4.358121, 3.032195, 1.631892, 0.7408397, -0.004113, -0.613288, -0.73758, -0.623346, -0.561229,
                            -0.29828, -0.21344, -0.322892, -0.386718, -0.534751, -0.874178, -1.231799, -1.503689, -1.896464, -2.385958])
    kwargs.setdefault("ToTCorrectionsEndcapXe"       , [0., 5.514777, 3.342712, 2.056626, 1.08293693, 0.3907979, -0.082819, -0.457485, -0.599706, -0.427493, 
                            -0.328962, -0.403399, -0.663656, -1.029428, -1.46008, -1.919092, -2.151582, -2.285481, -2.036822, -2.15805])
    kwargs.setdefault("ToTCorrectionsBarrelAr"       , [0., 4.358121, 3.032195, 1.631892, 0.7408397, -0.004113, -0.613288, -0.73758, -0.623346, -0.561229,
                            -0.29828, -0.21344, -0.322892, -0.386718, -0.534751, -0.874178, -1.231799, -1.503689, -1.896464, -2.385958])
    kwargs.setdefault("ToTCorrectionsEndcapAr"       , [0., 5.514777, 3.342712, 2.056626, 1.08293693, 0.3907979, -0.082819, -0.457485, -0.599706, -0.427493, 
                            -0.328962, -0.403399, -0.663656, -1.029428, -1.46008, -1.919092, -2.151582, -2.285481, -2.036822, -2.15805])
                    
    # Second calibration DB Service in case pile-up and physics hits have different calibrations
    if flags.Detector.RecoTRT:
        from InDetOverlay.TRT_ConditionsConfig import TRT_CalDbToolCfg
        InDetTRTCalDbTool2 = acc.popToolsAndMerge(TRT_CalDbToolCfg(flags))
        acc.addPublicTool(InDetTRTCalDbTool2)

        kwargs.setdefault("TRTCalDbTool2", InDetTRTCalDbTool2)
        kwargs.setdefault("IsOverlay", True)
        kwargs.setdefault("IsMC", False)
    acc.setPrivateTools(CompFactory.TRT_DriftFunctionTool(name, **kwargs))
    return acc

def TRT_DriftCircleToolCfg(flags, useTimeInfo, usePhase, prefix, name = "InDetTRT_DriftCircleTool", **kwargs):
    acc = ComponentAccumulator()
    #
    # --- TRT_DriftCircleTool
    #
    if usePhase:
        TRT_DriftCircleTool = CompFactory.InDet.TRT_DriftCircleToolCosmics
    else:
        TRT_DriftCircleTool = CompFactory.InDet.TRT_DriftCircleTool

    # set gating values for MC/DATA
    from AthenaCommon.SystemOfUnits import ns

    MinTrailingEdge = 11.0*ns
    MaxDriftTime    = 60.0*ns
    LowGate         = 14.0625*ns # 4.5*3.125 ns
    HighGate        = 42.1875*ns # LowGate + 9*3.125 ns
    if flags.Beam.Type == 'cosmics':
        LowGate         = 19.0*ns
        HighGate        = 44.0*ns
    if not flags.Input.isMC:
        MinTrailingEdge = 11.0*ns
        MaxDriftTime    = 60.0*ns
        LowGate         = 14.0625*ns ## 4.5*3.125 ns
        HighGate        = 42.1875*ns ## LowGate + 9*3.125 ns
        if flags.Beam.Type == 'cosmics':
            LowGate         = 19.0*ns
            HighGate        = 44.0*ns
    #
    # --- TRT_DriftFunctionTool
    #
    InDetTRT_DriftFunctionTool = acc.popToolsAndMerge(InDetTRT_DriftFunctionToolCfg(flags, useTimeInfo, usePhase, name = prefix + "DriftFunctionTool", **kwargs))
    acc.addPublicTool(InDetTRT_DriftFunctionTool)

    from InDetOverlay.TRT_ConditionsConfig import TRT_StrawStatusSummaryToolCfg
    InDetTRTStrawStatusSummaryTool = acc.popToolsAndMerge(TRT_StrawStatusSummaryToolCfg(flags))
    acc.addPublicTool(InDetTRTStrawStatusSummaryTool)

    kwargs.setdefault("TRTDriftFunctionTool", InDetTRT_DriftFunctionTool)
    kwargs.setdefault("ConditionsSummaryTool", InDetTRTStrawStatusSummaryTool)
    kwargs.setdefault("UseConditionsStatus", True)
    kwargs.setdefault("UseConditionsHTStatus", True)
    kwargs.setdefault("SimpleOutOfTimePileupSupression", flags.Beam.Type == 'cosmics') #flags.InDet.doCosmics
    kwargs.setdefault("RejectIfFirstBit", False)
    kwargs.setdefault("MinTrailingEdge", MinTrailingEdge)
    kwargs.setdefault("MaxDriftTime", MaxDriftTime)
    kwargs.setdefault("ValidityGateSuppression", flags.Beam.Type != 'cosmics') #not flags.InDet.doCosmics
    kwargs.setdefault("LowGate", LowGate)
    kwargs.setdefault("HighGate", HighGate)
    kwargs.setdefault("SimpleOutOfTimePileupSupressionArgon" , flags.Beam.Type == 'cosmics') #flags.InDet.doCosmics
    kwargs.setdefault("RejectIfFirstBitArgon", False)
    kwargs.setdefault("MinTrailingEdgeArgon", MinTrailingEdge)
    kwargs.setdefault("MaxDriftTimeArgon", MaxDriftTime)
    kwargs.setdefault("ValidityGateSuppressionArgon" , flags.Beam.Type != 'cosmics') #not flags.InDet.doCosmics
    kwargs.setdefault("LowGateArgon", LowGate)
    kwargs.setdefault("HighGateArgon", HighGate)
    kwargs.setdefault("useDriftTimeHTCorrection", True)
    kwargs.setdefault("useDriftTimeToTCorrection", True)


    if flags.InDet.InDet25nsec and flags.Beam.Type == "collisions":
        kwargs.setdefault("ValidityGateSuppression", True)
        kwargs.setdefault("SimpleOutOfTimePileupSupression", False)

    if flags.Beam.Type == "cosmics":
        kwargs.setdefault("SimpleOutOfTimePileupSupression", False)

    acc.setPrivateTools(TRT_DriftCircleTool(name, **kwargs))
    return acc

def TRT_LocalOccupancyCfg(flags, name = "InDet_TRT_LocalOccupancy", **kwargs):
    acc = ComponentAccumulator()
    # Calibration DB Service
    from InDetOverlay.TRT_ConditionsConfig import TRT_CalDbToolCfg
    InDetTRTCalDbTool = acc.popToolsAndMerge(TRT_CalDbToolCfg(flags))
    acc.addPublicTool(InDetTRTCalDbTool)
    # Straw status DB Tool
    from InDetOverlay.TRT_ConditionsConfig import TRT_StrawStatusSummaryToolCfg
    InDetTRTStrawStatusSummaryTool = acc.popToolsAndMerge(TRT_StrawStatusSummaryToolCfg(flags))
    acc.addPublicTool(InDetTRTStrawStatusSummaryTool)
    kwargs.setdefault("isTrigger", False)
    kwargs.setdefault("TRTCalDbTool", InDetTRTCalDbTool)
    kwargs.setdefault("TRTStrawStatusSummaryTool", InDetTRTStrawStatusSummaryTool)
    
    InDetTRT_LocalOccupancy = CompFactory.TRT_LocalOccupancy(name, **kwargs)
    acc.setPrivateTools(InDetTRT_LocalOccupancy)
    return acc

def TRTOccupancyIncludeCfg(flags, name = "InDetTRT_TRTOccupancyInclude", **kwargs):
    acc = ComponentAccumulator()
    # Calibration DB Service
    from InDetOverlay.TRT_ConditionsConfig import TRT_CalDbToolCfg
    InDetTRTCalDbTool = acc.popToolsAndMerge( TRT_CalDbToolCfg(flags) )
    acc.addPublicTool(InDetTRTCalDbTool)
    # Straw status DB Tool
    from InDetOverlay.TRT_ConditionsConfig import TRT_StrawStatusSummaryToolCfg
    InDetTRTStrawStatusSummaryTool = acc.popToolsAndMerge(TRT_StrawStatusSummaryToolCfg(flags))
    acc.addPublicTool(InDetTRTStrawStatusSummaryTool)
    InDetTRT_LocalOccupancy = acc.popToolsAndMerge(TRT_LocalOccupancyCfg(flags))
    acc.addPublicTool(InDetTRT_LocalOccupancy)
    kwargs.setdefault("isTrigger", False)
    kwargs.setdefault("TRTCalDbTool", InDetTRTCalDbTool)
    kwargs.setdefault("TRTStrawStatusSummaryTool", InDetTRTStrawStatusSummaryTool)
    kwargs.setdefault("TRT_LocalOccupancyTool", InDetTRT_LocalOccupancy)

    acc.addEventAlgo(CompFactory.TRTOccupancyInclude(name, **kwargs))
    return acc

def InDetTRT_RIO_MakerCfg(flags, useTimeInfo, usePhase, prefix, collection, name = "InDetTRT_RIO_Maker", **kwargs):
    acc = ComponentAccumulator()
    #
    # --- TRT_DriftCircleTool
    #
    InDetTRT_DriftCircleTool = acc.popToolsAndMerge(TRT_DriftCircleToolCfg(flags, useTimeInfo, usePhase, prefix, name = "InDetTRT_DriftCircleTool"))
    acc.addPublicTool(InDetTRT_DriftCircleTool)

    # Region selector tools for TRT
    from RegionSelector.RegSelToolConfig import regSelTool_TRT_Cfg
    RegSelTool_TRT = acc.popToolsAndMerge(regSelTool_TRT_Cfg(flags))
    #
    # --- TRT_RIO_Maker Algorithm
    #
    kwargs.setdefault("TRT_DriftCircleTool", InDetTRT_DriftCircleTool)
    kwargs.setdefault("TrtDescrManageLocation", 'TRT')
    kwargs.setdefault("TRTRDOLocation", 'TRT_RDOs')
    kwargs.setdefault("TRTRIOLocation", collection)
    kwargs.setdefault("RegSelTool", RegSelTool_TRT)

    acc.addEventAlgo(CompFactory.InDet.TRT_RIO_Maker(name, **kwargs))
    return acc

def InDetTRT_RIO_MakerPUCfg(flags, useTimeInfo, usePhase, prefix, collectionPU, name = "InDetTRT_RIO_MakerPU", **kwargs):
    acc = ComponentAccumulator()
    #
    # --- TRT_DriftCircleTool
    #
    InDetTRT_DriftCircleTool = acc.popToolsAndMerge(TRT_DriftCircleToolCfg(flags, useTimeInfo, usePhase, prefix, name = "InDetTRT_DriftCircleTool"))
    acc.addPublicTool(InDetTRT_DriftCircleTool)

    # Region selector tools for TRT
    from RegionSelector.RegSelToolConfig import regSelTool_TRT_Cfg
    RegSelTool_TRT = acc.popToolsAndMerge(regSelTool_TRT_Cfg(flags))
    #
    # --- TRT_RIO_Maker Algorithm
    #
    kwargs.setdefault("TRT_DriftCircleTool", InDetTRT_DriftCircleTool)
    kwargs.setdefault("TrtDescrManageLocation", 'TRT')
    kwargs.setdefault("TRTRDOLocation", 'TRT_PU_RDOs')
    kwargs.setdefault("TRTRIOLocation", collectionPU)
    kwargs.setdefault("RegSelTool", RegSelTool_TRT)

    acc.addEventAlgo(CompFactory.InDet.TRT_RIO_Maker(name, **kwargs))
    return acc
########################################################################################################
########################################################################################################
def TRTPreProcessingCfg(flags, useTimeInfo = True, usePhase = False, **kwargs):
    acc = ComponentAccumulator()

    from PixelConditionsAlgorithms.PixelConditionsConfig import (PixelChargeCalibCondAlgCfg, PixelConfigCondAlgCfg, PixelCablingCondAlgCfg, PixelReadoutSpeedAlgCfg)

    acc.merge(PixelChargeCalibCondAlgCfg(flags))
    acc.merge(PixelConfigCondAlgCfg(flags))
    acc.merge(PixelCablingCondAlgCfg(flags))
    acc.merge(PixelReadoutSpeedAlgCfg(flags))

    if flags.InDet.doPRDFormation and flags.InDet.doTRT_PRDFormation:
        #
        # --- setup naming of tools and algs
        #
        if useTimeInfo:
            prefix     = "InDetTRT_"
            collection = 'TRT_DriftCircles' ##InDetKeys.TRT_DriftCircles
            if flags.InDet.doSplitReco:
                collectionPU = 'TRT_PU_DriftCircles' ##read from InDetKeys.TRT_PU_DriftCircles
        else:
            prefix     = "InDetTRT_noTime_"
            collection = 'TRT_DriftCirclesUncalibrated' ##read from InDetKeys.TRT_DriftCirclesUncalibrated
            if flags.InDet.doSplitReco:
                collectionPU = 'TRT_PU_DriftCirclesUncalibrated' ##read from InDetKeys.TRT_PU_DriftCirclesUncalibrated
        #
        # --- TRT_RIO_Maker Algorithm
        #
        acc.merge(InDetTRT_RIO_MakerCfg(flags, useTimeInfo, usePhase, prefix, collection, name = prefix+"RIO_Maker"))
        if flags.InDet.doSplitReco :
            acc.merge(InDetTRT_RIO_MakerPUCfg(flags,  useTimeInfo, usePhase, prefix, collectionPU, name = prefix+"RIO_MakerPU", **kwargs))
        #
        #    Include alg to save the local occupancy inside xAOD::EventInfo
        #
        if flags.InDet.doTRTGlobalOccupancy:
            acc.merge(TRTOccupancyIncludeCfg(flags, name = prefix +"TRTOccupancyInclude"))
        #
        # --- we need to do truth association if requested (not for uncalibrated hits in cosmics)
        #
        if flags.InDet.doTruth and useTimeInfo:
            acc.merge(InDetPRD_MultiTruthMakerTRTCfg(flags, name = prefix + "PRD_MultiTruthMaker"))
            if flags.InDet.doSplitReco :
                acc.merge(InDetPRD_MultiTruthMakerTRTPUCfg(flags, name = prefix+"PRD_MultiTruthMakerPU"))
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files=defaultTestFiles.RDO

    ConfigFlags.Detector.RecoTRT = True

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads # Might change this later, but good enough for the moment.

    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    top_acc.merge(TRT_GeometryCfg( ConfigFlags ))

    msgService = top_acc.getService('MessageSvc')
    msgService.Format = "S:%s E:%e % F%138W%S%7W%R%T  %0W%M"

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    top_acc.merge( PixelGeometryCfg(ConfigFlags) )
    top_acc.merge( SCT_GeometryCfg(ConfigFlags) )

    if not ConfigFlags.InDet.doDBMstandalone:
        top_acc.merge(TRTPreProcessingCfg(ConfigFlags,(not ConfigFlags.InDet.doTRTPhaseCalculation or ConfigFlags.Beam.Type =="collisions"),False))
    
    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5
    top_acc.run(25)
    top_acc.store(open("test_TRTPrepocessing.pkl", "wb"))