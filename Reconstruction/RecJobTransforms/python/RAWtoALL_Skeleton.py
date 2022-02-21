# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude


def fromRunArgs(runArgs):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True

    from AthenaCommon.Logging import logging
    log = logging.getLogger('RAWtoALL')
    log.info('****************** STARTING Reconstruction (RAWtoALL) *****************')

    log.info('**** Transformation run arguments')
    log.info(str(runArgs))

    import time
    timeStart = time.time()

    log.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)
    from RecJobTransforms.RecoConfigFlags import recoRunArgsToFlags
    recoRunArgsToFlags(runArgs, ConfigFlags)

    # TODO: event service?

    ## Inputs
    # BS
    inputsDRAW = [prop for prop in dir(runArgs) if prop.startswith('inputDRAW') and prop.endswith('File')]
    if hasattr(runArgs, 'inputBSFile'):
        if inputsDRAW:
            raise RuntimeError('Impossible to run RAWtoESD with input BS and DRAW files (one input type only!)')
        ConfigFlags.Input.Files = runArgs.inputBSFile
    if len(inputsDRAW) == 1:
        ConfigFlags.Input.Files = getattr(runArgs, inputsDRAW[0])
    elif len(inputsDRAW) > 1:
        raise RuntimeError('Impossible to run RAWtoALL with multiple input DRAW files (viz.: {0})'.format(inputsDRAW))

    # RDO
    if hasattr(runArgs, 'inputRDOFile'):
        ConfigFlags.Input.Files = runArgs.inputRDOFile
    if hasattr(runArgs, 'inputRDO_TRIGFile'):
        ConfigFlags.Input.Files = runArgs.inputRDO_TRIGFile
        # TODO: trigger setup

    # TODO: EVNT?

    ## Outputs
    if hasattr(runArgs, 'outputESDFile'):
        ConfigFlags.Output.ESDFileName = runArgs.outputESDFile
        log.info("---------- Configured ESD output")

    if hasattr(runArgs, 'outputAODFile'):
        ConfigFlags.Output.AODFileName = runArgs.outputAODFile
        log.info("---------- Configured AOD output")

    from AthenaConfiguration.Enums import ProductionStep
    ConfigFlags.Common.ProductionStep=ProductionStep.Reconstruction

    # TODO: DESD, DAOD and DRAW

    # outputHIST_R2AFile
    # outputTAG_COMMFile
    # outputHISTFile
    # outputTXT_FTKIPFile
    # outputNTUP_MUONCALIBFile
    # outputTXT_JIVEXMLTGZFile

    # Setup perfmon flags from runargs
    from PerfMonComps.PerfMonConfigHelpers import setPerfmonFlagsFromRunArgs
    setPerfmonFlagsFromRunArgs(ConfigFlags, runArgs)

    # Pre-include
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    processPreExec(runArgs, ConfigFlags)

    # Lock flags
    ConfigFlags.lock()

    log.info("Configuring according to flag values listed below")
    ConfigFlags.dump()

    # Main reconstruction steering
    from RecJobTransforms.RecoSteering import RecoSteering
    cfg = RecoSteering(ConfigFlags)

    # Post-include
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    processPostExec(runArgs, ConfigFlags, cfg)

    timeConfig = time.time()
    log.info("configured in %d seconds", timeConfig - timeStart)


    # Run the final accumulator
    sc = cfg.run()
    timeFinal = time.time()
    log.info("Run RAWtoALL_skeleton in %d seconds (running %d seconds)", timeFinal - timeStart, timeFinal - timeConfig)

    import sys
    sys.exit(not sc.isSuccess())
