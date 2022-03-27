# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import sys
from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude
from SimuJobTransforms.CommonSimulationSteering import CommonSimulationCfg, specialConfigPreInclude, specialConfigPostInclude


def defaultReSimulationFlags(ConfigFlags, detectors):
    """Fill default resimulation flags"""
    ConfigFlags.Sim.ISF.ReSimulation = True

    # others are the same as standard simulation
    from SimuJobTransforms.ISF_Skeleton import defaultSimulationFlags
    defaultSimulationFlags(ConfigFlags, detectors)


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
    from G4AtlasApps.SimEnums import SimulationFlavour
    commonRunArgsToFlags(runArgs, ConfigFlags)

    # Generate detector list
    from SimuJobTransforms.SimulationHelpers import getDetectorsFromRunArgs
    detectors = getDetectorsFromRunArgs(ConfigFlags, runArgs)

    if hasattr(runArgs, 'simulator'):
       ConfigFlags.Sim.ISF.Simulator = SimulationFlavour(runArgs.simulator)

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
