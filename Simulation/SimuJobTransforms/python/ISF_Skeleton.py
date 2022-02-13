# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import sys
from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude
from SimuJobTransforms.CommonSimulationSteering import CommonSimulationCfg, specialConfigPreInclude, specialConfigPostInclude


def defaultSimulationFlags(ConfigFlags, detectors):
    """Fill default simulation flags"""
    # TODO: how to autoconfigure those
    from AthenaConfiguration.Enums import ProductionStep
    ConfigFlags.Common.ProductionStep = ProductionStep.Simulation
    # Writing out CalibrationHits only makes sense if we are running FullG4 simulation without frozen showers
    from G4AtlasApps.SimEnums import CalibrationRun, LArParameterization, SimulationFlavour
    if ConfigFlags.Sim.ISF.Simulator not in [SimulationFlavour.FullG4MT, SimulationFlavour.FullG4MT_QS, SimulationFlavour.PassBackG4MT] \
        or ConfigFlags.Sim.LArParameterization is not LArParameterization.NoFrozenShowers:
        ConfigFlags.Sim.CalibrationRun = CalibrationRun.Off

    ConfigFlags.Sim.RecordStepInfo = False
    ConfigFlags.Sim.ReleaseGeoModel = False
    ConfigFlags.Sim.ISFRun = True
    ConfigFlags.GeoModel.Align.Dynamic = False

    # Fatras does not support simulating the BCM, so have to switch that off
    if ConfigFlags.Sim.ISF.Simulator in [SimulationFlavour.ATLFASTIIFMT, SimulationFlavour.ATLFASTIIF_G4MS, SimulationFlavour.ATLFAST3F_G4MS]:
        try:
            detectors.remove('BCM')
        except ValueError:
            pass

    # Setup detector flags
    from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)


def fromRunArgs(runArgs):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaCommon.Logging import logging
    log = logging.getLogger('Sim_tf')
    log.info('****************** STARTING Simulation *****************')

    log.info('**** Transformation run arguments')
    log.info(str(runArgs))

    log.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.Enums import BeamType
    from G4AtlasApps.SimEnums import CalibrationRun, CavernBackground, SimulationFlavour
    commonRunArgsToFlags(runArgs, ConfigFlags)

    # Generate detector list
    from SimuJobTransforms.SimulationHelpers import getDetectorsFromRunArgs
    detectors = getDetectorsFromRunArgs(ConfigFlags, runArgs)

    if hasattr(runArgs, 'simulator'):
       ConfigFlags.Sim.ISF.Simulator = SimulationFlavour(runArgs.simulator)

    # Setup common simulation flags
    defaultSimulationFlags(ConfigFlags, detectors)

    # Beam Type
    if hasattr(runArgs,'beamType'):
        if runArgs.beamType == 'cosmics':
            ConfigFlags.Beam.Type = BeamType.Cosmics
            ConfigFlags.Sim.CavernBackground = CavernBackground.Off

    # Setup input: Three possible cases:
    # 1) inputEVNTFile (normal)
    # 2) inputEVNT_TRFile (TrackRecords from pre-simulated events,
    # used with TrackRecordGenerator)
    # 3) no input file (on-the-fly generation - typically ParticleGun
    # or CosmicGenerator)
    if hasattr(runArgs, 'inputEVNTFile'):
        ConfigFlags.Input.Files = runArgs.inputEVNTFile
    elif hasattr(runArgs, 'inputEVNT_TRFile'):
        ConfigFlags.Input.Files = runArgs.inputEVNT_TRFile
        # Three common cases here:
        # 2a) Cosmics simulation
        # 2b) Stopped particle simulation
        # 2c) Cavern background simulation
        if ConfigFlags.Beam.Type is BeamType.Cosmics:
            ConfigFlags.Sim.ReadTR = True
            ConfigFlags.Sim.CosmicFilterVolumeNames = ['Muon']
            ConfigFlags.Detector.GeometryCavern = True # simulate the cavern with a cosmic TR file
        elif hasattr(runArgs,"trackRecordType") and runArgs.trackRecordType=="stopped":
            ConfigFlags.Sim.ReadTR = True
            log.error('Stopped Particle simulation is not supported yet')
        else:
            ConfigFlags.Detector.GeometryCavern = True # simulate the cavern
            ConfigFlags.Sim.CavernBackground = CavernBackground.Read
    else:
        # Common cases
        # 3a) ParticleGun
        # 3b) CosmicGenerator
        ConfigFlags.Input.Files = []
        ConfigFlags.Input.isMC = True
        log.info('No inputEVNTFile provided. Assuming that you are running a generator on the fly.')
        if ConfigFlags.Beam.Type is BeamType.Cosmics:
            ConfigFlags.Sim.CosmicFilterVolumeNames = [getattr(runArgs, "CosmicFilterVolume", "InnerDetector")]
            ConfigFlags.Sim.CosmicFilterVolumeNames += [getattr(runArgs, "CosmicFilterVolume2", "NONE")]
            ConfigFlags.Sim.CosmicPtSlice = getattr(runArgs, "CosmicPtSlice", 'NONE')
            ConfigFlags.Detector.GeometryCavern = True # simulate the cavern when generating cosmics on-the-fly
            log.debug('No inputEVNTFile provided. OK, as performing cosmics simulation.')

    if hasattr(runArgs, 'outputHITSFile'):
        if runArgs.outputHITSFile == 'None':
            ConfigFlags.Output.HITSFileName = ''
        else:
            ConfigFlags.Output.HITSFileName  = runArgs.outputHITSFile
    if hasattr(runArgs, "outputEVNT_TRFile"):
        # Three common cases
        # 1b) Write TrackRecords for Cavern background
        # 1c) Write TrackRecords for Stopped particles
        # 3b) CosmicGenerator
        ConfigFlags.Output.EVNT_TRFileName = runArgs.outputEVNT_TRFile
        if hasattr(runArgs,"trackRecordType") and runArgs.trackRecordType=="stopped":
            # Case 1c)
            log.error('Stopped Particle simulation not supported yet!')
        elif ConfigFlags.Beam.Type is BeamType.Cosmics:
            # Case 3b)
            pass
        else:
            #Case 1b) Cavern Background
            ConfigFlags.Detector.GeometryCavern = True # simulate the cavern
            ConfigFlags.Sim.CalibrationRun = CalibrationRun.Off
            ConfigFlags.Sim.CavernBackground = CavernBackground.Write
    if not (hasattr(runArgs, 'outputHITSFile') or hasattr(runArgs, "outputEVNT_TRFile")):
        raise RuntimeError('No outputHITSFile or outputEVNT_TRFile defined')

    # Setup perfmon flags from runargs
    from SimuJobTransforms.SimulationHelpers import setPerfmonFlagsFromRunArgs
    setPerfmonFlagsFromRunArgs(ConfigFlags, runArgs)

    # Special Configuration preInclude
    specialConfigPreInclude(ConfigFlags)

    # Pre-include
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    processPreExec(runArgs, ConfigFlags)

    # Common simulation runtime arguments
    from G4AtlasApps.SimConfigFlags import simulationRunArgsToFlags
    simulationRunArgsToFlags(runArgs, ConfigFlags)

    # Lock flags
    ConfigFlags.lock()

    cfg = CommonSimulationCfg(ConfigFlags, log)

    cfg.getService("MessageSvc").Format = "% F%18W%S%7W%R%T %0W%M"

    # Special Configuration postInclude
    specialConfigPostInclude(ConfigFlags, cfg)

    # Post-include
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    processPostExec(runArgs, ConfigFlags, cfg)

    import time
    tic = time.time()
    # Run the final accumulator
    sc = cfg.run()
    log.info("Run ISF_MainConfigNew_Test in " + str(time.time()-tic) + " seconds")

    sys.exit(not sc.isSuccess())
