# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys

from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude


def defaultOverlayFlags(configFlags, detectors):
    """Fill default overlay flags"""
    # TODO: how to autoconfigure those
    configFlags.GeoModel.Align.Dynamic = False
    configFlags.Digitization.DoCaloNoise = False
    configFlags.Digitization.DoInnerDetectorNoise = False
    configFlags.Digitization.DoDigiTruth = False
    configFlags.LAr.OFCShapeFolder = "4samples1phase"
    configFlags.LAr.ROD.DoOFCPileupOptimization = True
    configFlags.LAr.ROD.nSamples = 4
    configFlags.LAr.ROD.NumberOfCollisions = 20
    configFlags.LAr.ROD.UseHighestGainAutoCorr = True
    configFlags.Tile.BestPhaseFromCOOL = False
    configFlags.Tile.correctTime = False
    configFlags.Tile.zeroAmplitudeWithoutDigits = False


def fromRunArgs(runArgs):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaCommon.Logging import logging
    logOverlay = logging.getLogger('Overlay')
    logOverlay.info('****************** STARTING OVERLAY *****************')

    logOverlay.info('**** Transformation run arguments')
    logOverlay.info(str(runArgs))

    logOverlay.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)

    hasRDO_BKGInput = hasattr(runArgs, 'inputRDO_BKGFile')
    hasBS_SKIMInput = hasattr(runArgs, 'inputBS_SKIMFile')

    if not hasattr(runArgs, 'inputHITSFile'):
        raise RuntimeError('No input HITS file defined')

    if hasRDO_BKGInput and hasBS_SKIMInput:
        raise RuntimeError('Both RDO_BKG and BS_SKIM are defined')
    if not hasRDO_BKGInput and not hasBS_SKIMInput:
        raise RuntimeError('Define one of RDO_BKG and BS_SKIM file types')

    if hasRDO_BKGInput:
        logOverlay.info('Running MC+MC overlay')
        ConfigFlags.Overlay.DataOverlay = False
        ConfigFlags.Input.isMC = True
        ConfigFlags.Input.Files = runArgs.inputRDO_BKGFile
        ConfigFlags.Input.SecondaryFiles = runArgs.inputHITSFile
    else:
        logOverlay.info('Running MC+data overlay')
        ConfigFlags.Overlay.DataOverlay = True
        ConfigFlags.Input.isMC = False
        ConfigFlags.Input.Files = runArgs.inputHITSFile
        ConfigFlags.Input.SecondaryFiles = runArgs.inputBS_SKIMFile

    if hasattr(runArgs, 'outputRDOFile'):
        if runArgs.outputRDOFile == 'None':
            ConfigFlags.Output.RDOFileName = ''
        else:
            ConfigFlags.Output.RDOFileName = runArgs.outputRDOFile
    else:
        raise RuntimeError('No output RDO file defined')

    if hasattr(runArgs, 'outputRDO_SGNLFile'):
        ConfigFlags.Output.RDO_SGNLFileName = runArgs.outputRDO_SGNLFile

    # Autoconfigure enabled subdetectors
    if hasattr(runArgs, 'detectors'):
        detectors = runArgs.detectors
    else:
        detectors = None

    # Setup digitization flags
    from Digitization.DigitizationConfigFlags import digitizationRunArgsToFlags
    digitizationRunArgsToFlags(runArgs, ConfigFlags)

    # Setup common overlay flags
    defaultOverlayFlags(ConfigFlags, detectors)

    from OverlayConfiguration.OverlaySteering import setupOverlayDetectorFlags
    setupOverlayDetectorFlags(ConfigFlags, detectors)

    # Pre-include
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    processPreExec(runArgs, ConfigFlags)

    # TODO not parsed yet:
    # '--fSampltag'
    # '--triggerConfig'

    # Lock flags
    ConfigFlags.lock()

    # Main overlay steering
    from OverlayConfiguration.OverlaySteering import OverlayMainCfg
    cfg = OverlayMainCfg(ConfigFlags)

    # Special message service configuration
    from Digitization.DigitizationSteering import DigitizationMessageSvcCfg
    cfg.merge(DigitizationMessageSvcCfg(ConfigFlags))

    # Post-include
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    processPostExec(runArgs, ConfigFlags, cfg)

    # Run the final configuration
    sc = cfg.run()
    sys.exit(not sc.isSuccess())
