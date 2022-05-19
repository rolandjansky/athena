# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import sys

from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude
from SimuJobTransforms.CommonSimulationSteering import specialConfigPreInclude, specialConfigPostInclude


def defaultOverlayFlags(configFlags):
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


def setOverlayInputFiles(runArgs, configFlags, log):
    hasRDO_BKGInput = hasattr(runArgs, 'inputRDO_BKGFile')
    hasBS_SKIMInput = hasattr(runArgs, 'inputBS_SKIMFile')

    if not hasattr(runArgs, 'inputHITSFile'):
        raise RuntimeError('No input HITS file defined')

    if hasRDO_BKGInput and hasBS_SKIMInput:
        raise RuntimeError('Both RDO_BKG and BS_SKIM are defined')
    if not hasRDO_BKGInput and not hasBS_SKIMInput:
        raise RuntimeError('Define one of RDO_BKG and BS_SKIM file types')

    if hasattr(runArgs, 'skipSecondaryEvents'):
        configFlags.Overlay.SkipSecondaryEvents = runArgs.skipSecondaryEvents

    if hasRDO_BKGInput:
        log.info('Running MC+MC overlay')
        configFlags.Overlay.DataOverlay = False
        configFlags.Input.isMC = True
        configFlags.Input.Files = runArgs.inputRDO_BKGFile
        configFlags.Input.SecondaryFiles = runArgs.inputHITSFile
    else:
        log.info('Running MC+data overlay')
        configFlags.Overlay.DataOverlay = True
        configFlags.Input.isMC = False
        configFlags.Input.Files = runArgs.inputHITSFile
        configFlags.Input.SecondaryFiles = runArgs.inputBS_SKIMFile


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

    # Setting input files for Overlay
    setOverlayInputFiles(runArgs, ConfigFlags, logOverlay)

    from AthenaConfiguration.Enums import ProductionStep
    ConfigFlags.Common.ProductionStep = ProductionStep.Overlay

    # Setting output files for Overlay
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
    defaultOverlayFlags(ConfigFlags)

    # Setup detector flags
    if detectors:
        from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
        setupDetectorsFromList(ConfigFlags, detectors)

    # Disable LVL1 trigger if triggerConfig explicitly set to 'NONE'
    if hasattr(runArgs, 'triggerConfig') and runArgs.triggerConfig == 'NONE':
        ConfigFlags.Detector.EnableL1Calo = False

    # Setup perfmon flags from runargs
    from PerfMonComps.PerfMonConfigHelpers import setPerfmonFlagsFromRunArgs
    setPerfmonFlagsFromRunArgs(ConfigFlags, runArgs)

    # Special Configuration preInclude
    specialConfigPreInclude(ConfigFlags)

    # Pre-include
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    processPreExec(runArgs, ConfigFlags)

    # TODO not parsed yet:
    # '--fSampltag'

    # Lock flags
    ConfigFlags.lock()

    # Main overlay steering
    from OverlayConfiguration.OverlaySteering import OverlayMainCfg
    cfg = OverlayMainCfg(ConfigFlags)

    # Special message service configuration
    from Digitization.DigitizationSteering import DigitizationMessageSvcCfg
    cfg.merge(DigitizationMessageSvcCfg(ConfigFlags))

    # Special Configuration postInclude
    specialConfigPostInclude(ConfigFlags, cfg)

    # Post-include
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    processPostExec(runArgs, ConfigFlags, cfg)

    # Run the final configuration
    sc = cfg.run()
    sys.exit(not sc.isSuccess())
