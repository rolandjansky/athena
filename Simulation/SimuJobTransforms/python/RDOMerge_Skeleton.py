# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys
from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude


def fromRunArgs(runArgs):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaCommon.Logging import logging
    log = logging.getLogger('RDOMerge_tf')
    log.info('****************** STARTING RDO MERGING *****************')

    log.info('**** Transformation run arguments')
    log.info(str(runArgs))

    log.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)

    if hasattr(runArgs, "PileUpPresampling"):
        from AthenaConfiguration.Enums import ProductionStep
        ConfigFlags.Common.ProductionStep = ProductionStep.PileUpPresampling

    if hasattr(runArgs, 'inputRDOFile'):
        ConfigFlags.Input.Files = runArgs.inputRDOFile
    else:
        raise RuntimeError('No input RDO file defined')

    if hasattr(runArgs, 'outputRDO_MRGFile'):
        if runArgs.outputRDO_MRGFile == 'None':
            ConfigFlags.Output.RDOFileName = ''
            # TODO decide if we need a specific RDO_MRGFileName flag
        else:
            ConfigFlags.Output.RDOFileName  = runArgs.outputRDO_MRGFile
    else:
        raise RuntimeError('No outputRDO_MRGFile defined')

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

    # Geometry dependencies
    if ConfigFlags.Detector.EnablePixel:
        from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
        cfg.merge(PixelReadoutGeometryCfg(ConfigFlags))
    if ConfigFlags.Detector.EnableSCT:
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
        cfg.merge(SCT_ReadoutGeometryCfg(ConfigFlags))
    if ConfigFlags.Detector.EnableTRT:
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
        cfg.merge(TRT_ReadoutGeometryCfg(ConfigFlags))

    if ConfigFlags.Detector.EnableITkPixel:
        from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
        cfg.merge(ITkPixelReadoutGeometryCfg(ConfigFlags))
    if ConfigFlags.Detector.EnableITkStrip:
        from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
        cfg.merge(ITkStripReadoutGeometryCfg(ConfigFlags))

    if ConfigFlags.Detector.EnableHGTD:
        from HGTD_GeoModel.HGTD_GeoModelConfig import HGTD_ReadoutGeometryCfg
        cfg.merge(HGTD_ReadoutGeometryCfg(ConfigFlags))

    if ConfigFlags.Detector.EnableLAr:
        from LArGeoAlgsNV.LArGMConfig import LArGMCfg
        cfg.merge(LArGMCfg(ConfigFlags))
    if ConfigFlags.Detector.EnableTile:
        from TileGeoModel.TileGMConfig import TileGMCfg
        cfg.merge(TileGMCfg(ConfigFlags))
        from TileConditions.TileCablingSvcConfig import TileCablingSvcCfg
        cfg.merge(TileCablingSvcCfg(ConfigFlags))

    if ConfigFlags.Detector.EnableMuon:
        from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
        cfg.merge(MuonGeoModelCfg(ConfigFlags))
        if ConfigFlags.Detector.EnableRPC:
            from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
            cfg.merge(RPCCablingConfigCfg(ConfigFlags))

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(ConfigFlags, 'RDO'))
    cfg.getEventAlgo('OutputStreamRDO').TakeItemsFromInput = True

    # Silence HepMcParticleLink warnings
    from Digitization.DigitizationSteering import DigitizationMessageSvcCfg
    cfg.merge(DigitizationMessageSvcCfg(ConfigFlags))

    # Post-include
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    processPostExec(runArgs, ConfigFlags, cfg)

    import time
    tic = time.time()
    # Run the final accumulator
    sc = cfg.run()
    log.info("Ran RDOMerge_tf in " + str(time.time()-tic) + " seconds")

    sys.exit(not sc.isSuccess())
