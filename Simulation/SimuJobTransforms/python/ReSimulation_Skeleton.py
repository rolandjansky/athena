# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys
from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude
from SimuJobTransforms.CommonSimulationSteering import CommonSimulationCfg, specialConfigPreInclude, specialConfigPostInclude


def defaultReSimulationFlags(ConfigFlags, detectors):
    """Fill default simulation flags"""
    # TODO: how to autoconfigure those
    from AthenaConfiguration.Enums import ProductionStep
    ConfigFlags.Common.ProductionStep = ProductionStep.Simulation
    # Writing out CalibrationHits only makes sense if we are running FullG4 simulation without frozen showers
    if (ConfigFlags.Sim.ISF.Simulator not in ('FullG4MT', 'FullG4MT_LongLived')) or ConfigFlags.Sim.LArParameterization!=0:
        ConfigFlags.Sim.CalibrationRun = "Off"

    ConfigFlags.Sim.RecordStepInfo = False
    ConfigFlags.Sim.ReleaseGeoModel = False
    ConfigFlags.Sim.ISFRun = True
    ConfigFlags.Sim.ISF.ReSimulation = True
    ConfigFlags.GeoModel.Align.Dynamic = False

    #Frozen showers OFF = 0
    # ConfigFlags.Sim.LArParameterization = 2

    # Fatras does not support simulating the BCM, so have to switch that off
    if ConfigFlags.Sim.ISF.Simulator in ('ATLFASTIIF', 'ATLFASTIIFMT', 'ATLFASTIIF_G4MS', 'ATLFAST3F_G4MS'):
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
    commonRunArgsToFlags(runArgs, ConfigFlags)

    # Generate detector list
    from SimuJobTransforms.SimulationHelpers import getDetectorsFromRunArgs
    detectors = getDetectorsFromRunArgs(ConfigFlags, runArgs)

    if hasattr(runArgs, 'simulator'):
       ConfigFlags.Sim.ISF.Simulator = runArgs.simulator

    # Setup common simulation flags
    defaultReSimulationFlags(ConfigFlags, detectors)

    if hasattr(runArgs, 'inputHITSFile'):
        ConfigFlags.Input.Files = runArgs.inputHITSFile
    else:
        log.error('No inputHITSFile provided. Please try using Sim_tf.py instead.')
        raise RuntimeError('No intputHITSFile provided.')

    if hasattr(runArgs, 'outputHITS_RSMFile'):
        if runArgs.outputHITS_RSMFile == 'None':
            ConfigFlags.Output.HITSFileName = ''
        else:
            ConfigFlags.Output.HITSFileName  = runArgs.outputHITS_RSMFile

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

    # Add OLD suffix to the names of collections read in from the input HITS file
    from SimuJobTransforms.ReSimInputConfig import RenameHitCollectionsOnReadCfg
    cfg.merge(RenameHitCollectionsOnReadCfg(ConfigFlags))

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
