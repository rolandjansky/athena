# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys

from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude


def fromRunArgs(runArgs):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaCommon.Logging import logging
    log = logging.getLogger('HITtoRDO')
    log.info('****************** STARTING HITtoRDO *****************')

    log.info('**** Transformation run arguments')
    log.info(str(runArgs))

    log.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)

    if not hasattr(runArgs, 'inputHITSFile'):
        raise RuntimeError('No input HITS file defined')

    if not hasattr(runArgs, 'outputRDOFile'):
        raise RuntimeError('No output RDO file defined')

    ConfigFlags.Input.isMC = True
    ConfigFlags.Input.Files = runArgs.inputHITSFile
    ConfigFlags.Output.RDOFileName = runArgs.outputRDOFile

    # Autoconfigure enabled subdetectors
    if hasattr(runArgs, 'detectors'):
        detectors = runArgs.detectors
    else:
        detectors = None

    # Setup digitization flags
    from Digitization.DigitizationConfigFlags import digitizationRunArgsToFlags, pileupRunArgsToFlags
    digitizationRunArgsToFlags(runArgs, ConfigFlags)
    pileupRunArgsToFlags(runArgs, ConfigFlags)

    # Setup common digitization flags
    from Digitization.DigitizationSteering import setupDigitizationFlags
    setupDigitizationFlags(ConfigFlags)

    # Setup detector flags
    ConfigFlags.Digitization.TruthOutput = True  # temporary
    if detectors:
        from AthenaConfiguration.DetectorConfigFlags import setupDetectorsFromList
        setupDetectorsFromList(ConfigFlags, detectors)

    # Pre-include
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    processPreExec(runArgs, ConfigFlags)

    # TODO not parsed yet:
    # '--outputRDO_FILTFile'

    # Lock flags
    ConfigFlags.lock()

    # Main overlay steering
    from Digitization.DigitizationSteering import DigitizationMainCfg
    cfg = DigitizationMainCfg(ConfigFlags)

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
