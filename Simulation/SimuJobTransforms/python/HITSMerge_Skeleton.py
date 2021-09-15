# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys
from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude


def fromRunArgs(runArgs):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaCommon.Logging import logging
    log = logging.getLogger('HITSMerge_tf')
    log.info('****************** STARTING HIT MERGING *****************')

    log.info('**** Transformation run arguments')
    log.info(str(runArgs))

    log.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)

    if hasattr(runArgs, 'inputHITSFile'):
        ConfigFlags.Input.Files = runArgs.inputHITSFile
    else:
        raise RuntimeError('No input HITS file defined')

    if hasattr(runArgs, 'outputHITS_MRGFile'):
        if runArgs.outputHITS_MRGFile == 'None':
            ConfigFlags.Output.HITSFileName = ''
            # TODO decide if we need a specific HITS_MRGFileName flag
        else:
            ConfigFlags.Output.HITSFileName  = runArgs.outputHITS_MRGFile
    else:
        raise RuntimeError('No outputHITS_MRGFile defined')

    # Generate detector list and setup detector flags
    from SimuJobTransforms.SimulationHelpers import getDetectorsFromRunArgs
    detectors = getDetectorsFromRunArgs(ConfigFlags, runArgs)
    from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
    setupDetectorsFromList(ConfigFlags, detectors, toggle_geometry=True)

    # Pre-include
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    processPreExec(runArgs, ConfigFlags)

    # Lock flags
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(PoolWriteCfg(ConfigFlags))

    # Minor geometry dependencies
    if ConfigFlags.Detector.GeometryLAr:
        from LArGeoAlgsNV.LArGMConfig import LArGMCfg
        cfg.merge(LArGMCfg(ConfigFlags))
    if ConfigFlags.Detector.GeometryTile:
        from TileGeoModel.TileGMConfig import TileGMCfg
        cfg.merge(TileGMCfg(ConfigFlags))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(ConfigFlags, 'HITS', disableEventTag=True))
    cfg.getEventAlgo('OutputStreamHITS').TakeItemsFromInput = True

    # Post-include
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    processPostExec(runArgs, ConfigFlags, cfg)

    import time
    tic = time.time()
    # Run the final accumulator
    sc = cfg.run()
    log.info("Ran HITSMerge_tf in " + str(time.time()-tic) + " seconds")

    sys.exit(not sc.isSuccess())
