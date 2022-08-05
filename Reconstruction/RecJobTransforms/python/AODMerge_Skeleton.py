# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def fromRunArgs(runArgs):

    # Setup logging
    from AthenaCommon.Logging import logging
    log = logging.getLogger('AODMerge_tf')
    log.info( '****************** STARTING AOD MERGING *****************' )

    # Print arguments
    log.info('**** Transformation run arguments')
    log.info(str(runArgs))

    # Setup configuration flags
    log.info('**** Setting up configuration flags')

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True

    from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)

    if hasattr(runArgs, 'inputAODFile'):
        ConfigFlags.Input.Files = runArgs.inputAODFile
    else:
        raise RuntimeError('Please provide an input AOD file (via --inputAODFile)')

    if hasattr(runArgs, 'outputAOD_MRGFile'):
        ConfigFlags.Output.AODFileName = runArgs.outputAOD_MRGFile
    else:
        raise RuntimeError('Please provide an output AOD file (via --outputAOD_MRGFile)')

    # Setup perfmon flags from runargs
    from PerfMonComps.PerfMonConfigHelpers import setPerfmonFlagsFromRunArgs
    setPerfmonFlagsFromRunArgs(ConfigFlags, runArgs)

    # Pre-include
    from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude
    log.info('**** Processing preInclude')
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    log.info('**** Processing preExec')
    processPreExec(runArgs, ConfigFlags)

    # Lock configuration flags
    log.info('**** Locking configuration flags')
    ConfigFlags.lock()

    # Set up necessary job components
    log.info('**** Setting up job components')

    # Main services
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    # Input reading
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    # Output writing
    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
    cfg.merge(PoolWriteCfg(ConfigFlags))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(ConfigFlags, 'AOD'))
    StreamAOD = cfg.getEventAlgo('OutputStreamAOD')
    StreamAOD.ForceRead = True
    StreamAOD.TakeItemsFromInput = True

    # This part is needed for (un)packing cell containers
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    cfg.merge(LArGMCfg(ConfigFlags))
    from TileGeoModel.TileGMConfig import TileGMCfg
    cfg.merge(TileGMCfg(ConfigFlags))

    # Add PerfMon
    if ConfigFlags.PerfMon.doFastMonMT or ConfigFlags.PerfMon.doFullMonMT:
        from PerfMonComps.PerfMonCompsConfig import PerfMonMTSvcCfg
        cfg.merge(PerfMonMTSvcCfg(ConfigFlags))

    # Set EventPrintoutInterval to 100 events
    cfg.getService(cfg.getAppProps()["EventLoop"]).EventPrintoutInterval = 100

    # Post-include
    log.info('**** Processing postInclude')
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    log.info('**** Processing postExec')
    processPostExec(runArgs, ConfigFlags, cfg)

    # Now run the job and exit accordingly
    sc = cfg.run()
    import sys
    sys.exit(not sc.isSuccess())
