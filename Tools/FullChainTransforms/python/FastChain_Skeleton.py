# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import sys
from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg


def fromRunArgs(runArgs):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaCommon.Logging import logging
    log = logging.getLogger('FastChain_Skeleton (CA based)')
    log.info('****************** STARTING FastChain Simulation *****************')

    log.info('**** Transformation run arguments')
    log.info(str(runArgs))

    log.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)

    # Autoconfigure enabled subdetectors
    if hasattr(runArgs, 'detectors'):
         detectors = runArgs.detectors
    else:
         from AthenaConfiguration.AutoConfigFlags import getDefaultDetectors
         detectors = getDefaultDetectors(ConfigFlags.GeoModel.AtlasVersion)

    # Support switching on simulation of Forward Detectors
    if hasattr(runArgs, 'LucidOn'):
        detectors = detectors+['Lucid']
    if hasattr(runArgs, 'ZDCOn'):
        detectors = detectors+['ZDC']
    if hasattr(runArgs, 'ALFAOn'):
        detectors = detectors+['ALFA']
    if hasattr(runArgs, 'FwdRegionOn'):
        detectors = detectors+['FwdRegion'] 

    if hasattr(runArgs, 'simulator'):
        ConfigFlags.Sim.ISF.Simulator = runArgs.simulator

    # Setup common simulation flags
    from SimuJobTransforms.ISF_Skeleton import defaultSimulationFlags
    defaultSimulationFlags(ConfigFlags, detectors)

    # Setup digitization flags
    from Digitization.DigitizationConfigFlags import digitizationRunArgsToFlags, pileupRunArgsToFlags
    digitizationRunArgsToFlags(runArgs, ConfigFlags)
    pileupRunArgsToFlags(runArgs, ConfigFlags)
 
    # Setup common digitization flags
    from Digitization.DigitizationSteering import setupDigitizationFlags
    setupDigitizationFlags(ConfigFlags)

    if hasattr(runArgs, 'inputEVNTFile'):
        ConfigFlags.Input.Files = runArgs.inputEVNTFile
    else:
        raise RuntimeError('No input EVNT file defined')

    if hasattr(runArgs, 'outputRDOFile'):
        if runArgs.outputRDOFile == 'None':
            ConfigFlags.Output.RDOFileName = ''
        else:
            ConfigFlags.Output.RDOFileName = runArgs.outputRDOFile
    else:
        raise RuntimeError('No outputRDOFile defined')

    if hasattr(runArgs, 'DataRunNumber'):
        ConfigFlags.Input.RunNumber = [runArgs.DataRunNumber] # is it updating?
        ConfigFlags.Input.OverrideRunNumber = True
        ConfigFlags.Input.LumiBlockNumber = [1] # dummy value

    if hasattr(runArgs, 'conditionsTag'):
        ConfigFlags.IOVDb.GlobalTag = runArgs.conditionsTag

    if hasattr(runArgs, 'truthStrategy'):
        ConfigFlags.Sim.TruthStrategy = runArgs.truthStrategy

    from AthenaConfiguration.Enums import ProductionStep
    ConfigFlags.Common.ProductionStep = ProductionStep.Simulation

    # Pre-include
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    processPreExec(runArgs, ConfigFlags)

    # Lock flags
    ConfigFlags.lock()

    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(PoolWriteCfg(ConfigFlags))

    from BeamEffects.BeamEffectsAlgConfig import BeamEffectsAlgCfg
    cfg.merge(BeamEffectsAlgCfg(ConfigFlags))

    from ISF_Algorithms.CollectionMergerConfig import ISFCollectionMergerCfg
    algo = ISFCollectionMergerCfg(ConfigFlags)
    cfg.addEventAlgo(algo)

    from ISF_Config.ISF_MainConfigNew import ISF_KernelCfg
    cfg.merge(ISF_KernelCfg(ConfigFlags))

    # Post-include
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    processPostExec(runArgs, ConfigFlags, cfg)


    # Run the final accumulator
    sc = cfg.run()
    sys.exit(not sc.isSuccess())
