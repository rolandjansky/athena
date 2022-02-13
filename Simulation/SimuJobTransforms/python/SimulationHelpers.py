# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from G4AtlasApps.SimEnums import BeamPipeSimMode, CalibrationRun, CavernBackground, LArParameterization


def setPerfmonFlagsFromRunArgs(ConfigFlags, runArgs):
    """ A helper function to set perfmon flags from runArgs."""

    if hasattr(runArgs, 'perfmon') and runArgs.perfmon != 'none':
        if runArgs.perfmon == 'fastmonmt':
            ConfigFlags.PerfMon.doFastMonMT = True
        elif runArgs.perfmon == 'fullmonmt':
            ConfigFlags.PerfMon.doFullMonMT = True
        else:
            raise RuntimeError(f"Unknown perfmon type: {runArgs.perfmon}")
        ConfigFlags.PerfMon.OutputJSON = f"perfmonmt_{runArgs.trfSubstepName}.json"


def getDetectorsFromRunArgs(ConfigFlags, runArgs):
    """Generate detector list based on runtime arguments."""
    if hasattr(runArgs, 'detectors'):
        detectors = runArgs.detectors
    else:
        from AthenaConfiguration.AutoConfigFlags import getDefaultDetectors
        detectors = getDefaultDetectors(ConfigFlags.GeoModel.AtlasVersion)

    # Support switching on Forward Detectors
    if hasattr(runArgs, 'LucidOn'):
        detectors = detectors+['Lucid']
    if hasattr(runArgs, 'ZDCOn'):
        detectors = detectors+['ZDC']
    if hasattr(runArgs, 'AFPOn'):
        detectors = detectors+['AFP']
    if hasattr(runArgs, 'ALFAOn'):
        detectors = detectors+['ALFA']
    if hasattr(runArgs, 'FwdRegionOn'):
        detectors = detectors+['FwdRegion']
    # TODO here support switching on Cavern geometry
    # if hasattr(runArgs, 'CavernOn'):
    #     detectors = detectors+['Cavern']

    return detectors


def enableFrozenShowersFCalOnly(ConfigFlags):
    """Turns on GFlash shower parametrization for FCAL"""
    ConfigFlags.Sim.LArParameterization = LArParameterization.FrozenShowersFCalOnly
    ConfigFlags.Sim.CalibrationRun = CalibrationRun.Off


def enableBeamPipeKill(ConfigFlags):
    ConfigFlags.Sim.BeamPipeCut = 0.
    ConfigFlags.Sim.BeamPipeSimMode = BeamPipeSimMode.FastSim


def enableTightMuonStepping(ConfigFlags):
    ConfigFlags.Sim.TightMuonStepping = True


def enableG4SignalCavern(ConfigFlags):
    """Set ConfigFlags to take care of Neutron BG"""
    ConfigFlags.Sim.CavernBackground = CavernBackground.Signal


def enableCalHits(ConfigFlags):
    """Turns on calibration hits for LAr and Tile"""
    ConfigFlags.Sim.CalibrationRun = CalibrationRun.LArTile


def enableParticleID(ConfigFlags):
    """Mods to have primary particle barcode signature on for calorimeter calibration hits."""
    ConfigFlags.Sim.ParticleID=True


def enableVerboseSelector(ConfigFlags):
    """ """
    ConfigFlags.Sim.OptionalUserActionList += ['G4DebuggingTools.G4DebuggingToolsConfigNew.VerboseSelectorToolCfg']
