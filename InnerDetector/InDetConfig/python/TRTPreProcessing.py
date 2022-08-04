# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# ------------------------------------------------------------
#
# ----------- TRT Data-Preparation stage
#
# ------------------------------------------------------------
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def InDetTRT_DriftFunctionToolCfg(flags, name = "InDetTRT_DriftFunctionTool", **kwargs):
    acc = ComponentAccumulator()
    #
    # --- TRT_DriftFunctionTool
    #
    # Calibration DB Service
    from TRT_ConditionsServices.TRT_ConditionsServicesConfig import TRT_CalDbToolCfg
    CalDbTool = acc.popToolsAndMerge(TRT_CalDbToolCfg(flags))
    acc.addPublicTool(CalDbTool)

    # --- overwrite for uncalibrated DC production
    if flags.Beam.Type is BeamType.Cosmics or flags.InDet.noTRTTiming:
        kwargs.setdefault("DummyMode", True)
        kwargs.setdefault("UniversalError", 1.15)

    # --- set Data/MC flag
    kwargs.setdefault("IsMC", flags.Input.isMC)

    kwargs.setdefault("TRTCalDbTool", CalDbTool)
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

    # Second calibration DB Service in case pile-up and physics hits have different calibrations for data overlay
    if flags.Overlay.DataOverlay:
        from TRT_ConditionsServices.TRT_ConditionsServicesConfig import TRT_MCCalDbToolCfg
        MCCalDbTool = acc.popToolsAndMerge(TRT_MCCalDbToolCfg(flags))
        acc.addPublicTool(MCCalDbTool)

        kwargs.setdefault("TRTCalDbTool2", MCCalDbTool)
        kwargs.setdefault("IsOverlay", True)
        kwargs.setdefault("IsMC", False)
    acc.setPrivateTools(CompFactory.TRT_DriftFunctionTool(name, **kwargs))
    return acc


def TRTDriftTimes(flags):
    from collections import namedtuple
    from AthenaCommon.SystemOfUnits import ns
    driftTimes = namedtuple("driftTimes", ("LowGate", "HighGate", "LowGateArgon", "HighGateArgon"))
    if flags.Beam.Type is BeamType.Cosmics:
        return driftTimes(LowGate         = 19.0*ns,
                          HighGate        = 44.0*ns,
                          LowGateArgon    = 19.0*ns,
                          HighGateArgon   = 44.0*ns)
    if not flags.Input.isMC:
        return driftTimes(LowGate         = 17.1875*ns,
                          HighGate        = 45.3125*ns,
                          LowGateArgon    = 18.75*ns,
                          HighGateArgon   = 43.75*ns)
    # MC
    return driftTimes(LowGate         = 14.0625*ns, # 4.5*3.125 ns
                      HighGate        = 42.1875*ns, # LowGate + 9*3.125 ns
                      LowGateArgon    = 14.0625*ns,
                      HighGateArgon   = 42.1875*ns)


def TRT_DriftCircleToolCfg(flags, name = "InDetTRT_DriftCircleTool", **kwargs):
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTAlignCondAlgCfg
    acc = TRTAlignCondAlgCfg(flags)
    #
    # --- TRT_DriftCircleTool
    #
    TRT_DriftCircleTool = CompFactory.InDet.TRT_DriftCircleTool

    # set gating values for MC/DATA
    from AthenaCommon.SystemOfUnits import ns

    ## sync with: ConfiguredInDetPreProcessingTRT.py
    MinTrailingEdge = 11.0*ns
    MaxDriftTime    = 60.0*ns
    gains = TRTDriftTimes(flags)
    #
    # --- TRT_DriftFunctionTool
    #
    InDetTRT_DriftFunctionTool = acc.popToolsAndMerge(InDetTRT_DriftFunctionToolCfg(flags, **kwargs))
    acc.addPublicTool(InDetTRT_DriftFunctionTool)

    from TRT_ConditionsServices.TRT_ConditionsServicesConfig import TRT_StrawStatusSummaryToolCfg
    InDetTRTStrawStatusSummaryTool = acc.popToolsAndMerge(TRT_StrawStatusSummaryToolCfg(flags))
    acc.addPublicTool(InDetTRTStrawStatusSummaryTool)

    kwargs.setdefault("TRTDriftFunctionTool", InDetTRT_DriftFunctionTool)
    kwargs.setdefault("ConditionsSummaryTool", InDetTRTStrawStatusSummaryTool)
    kwargs.setdefault("UseConditionsStatus", True)
    kwargs.setdefault("UseConditionsHTStatus", True)
    kwargs.setdefault("SimpleOutOfTimePileupSupression", flags.Beam.Type is BeamType.Cosmics)
    kwargs.setdefault("RejectIfFirstBit", False)
    kwargs.setdefault("MinTrailingEdge", MinTrailingEdge)
    kwargs.setdefault("MaxDriftTime", MaxDriftTime)
    kwargs.setdefault("ValidityGateSuppression", flags.Beam.Type is not BeamType.Cosmics)
    kwargs.setdefault("LowGate", gains.LowGate)
    kwargs.setdefault("HighGate", gains.HighGate)
    kwargs.setdefault("SimpleOutOfTimePileupSupressionArgon" , flags.Beam.Type is BeamType.Cosmics)
    kwargs.setdefault("RejectIfFirstBitArgon", False)
    kwargs.setdefault("MinTrailingEdgeArgon", MinTrailingEdge)
    kwargs.setdefault("MaxDriftTimeArgon", MaxDriftTime)
    kwargs.setdefault("ValidityGateSuppressionArgon" , flags.Beam.Type is not BeamType.Cosmics)
    kwargs.setdefault("LowGateArgon", gains.LowGate)  # see discussion in MR !45402 why these are not Argon specific settings
    kwargs.setdefault("HighGateArgon", gains.HighGate)
    kwargs.setdefault("useDriftTimeHTCorrection", True)
    kwargs.setdefault("useDriftTimeToTCorrection", True)


    if flags.Beam.BunchSpacing<=25 and flags.Beam.Type is BeamType.Collisions:
        kwargs.setdefault("ValidityGateSuppression", True)
        kwargs.setdefault("SimpleOutOfTimePileupSupression", False)

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("SimpleOutOfTimePileupSupression", False)

    acc.setPrivateTools(TRT_DriftCircleTool(name, **kwargs))
    return acc

########################################################################################################
########################################################################################################
def TRTPreProcessingCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    from AthenaConfiguration.Enums import Format
    if flags.Input.Format is Format.BS or 'TRT_RDOs' in flags.Input.Collections:

        #
        # --- TRT_RIO_Maker Algorithm
        #
        from InDetConfig.InDetPrepRawDataFormationConfig import InDetTRT_RIO_MakerCfg
        acc.merge(InDetTRT_RIO_MakerCfg(flags))
        if flags.InDet.doSplitReco:
            from InDetConfig.InDetPrepRawDataFormationConfig import InDetTRT_RIO_MakerPUCfg
            acc.merge(InDetTRT_RIO_MakerPUCfg(flags))

        #
        #    Include alg to save the local occupancy inside xAOD::EventInfo
        #
        if flags.InDet.doTRTGlobalOccupancy:
            from InDetConfig.TRT_ElectronPidToolsConfig import TRTOccupancyIncludeCfg
            acc.merge(TRTOccupancyIncludeCfg(flags))

    #
    # --- we need to do truth association if requested (not for uncalibrated hits in cosmics)
    #
    if flags.InDet.doTruth and flags.Beam.Type is not BeamType.Cosmics and 'PRD_MultiTruthTRT' not in flags.Input.Collections:
        from InDetConfig.InDetTruthAlgsConfig import InDetPRD_MultiTruthMakerTRTCfg
        acc.merge(InDetPRD_MultiTruthMakerTRTCfg(flags))
        if flags.InDet.doSplitReco :
            from InDetConfig.InDetTruthAlgsConfig import InDetPRD_MultiTruthMakerTRTPUCfg
            acc.merge(InDetPRD_MultiTruthMakerTRTPUCfg(flags))
    return acc


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files=defaultTestFiles.RDO_RUN2

    # TODO: TRT only?

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads # Might change this later, but good enough for the moment.

    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
    top_acc.merge(TRT_ReadoutGeometryCfg( ConfigFlags ))

    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    top_acc.merge( PixelReadoutGeometryCfg(ConfigFlags) )
    top_acc.merge( SCT_ReadoutGeometryCfg(ConfigFlags) )

    if not ConfigFlags.InDet.Tracking.doDBMstandalone:
        top_acc.merge(TRTPreProcessingCfg(ConfigFlags))

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5
    top_acc.run(25)
    top_acc.store(open("test_TRTPrepocessing.pkl", "wb"))
