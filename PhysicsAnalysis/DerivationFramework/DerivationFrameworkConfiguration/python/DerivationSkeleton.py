# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import sys

from PyJobTransforms.CommonRunArgsToFlags import commonRunArgsToFlags
from DerivationFrameworkConfiguration import DerivationConfigList
from PyJobTransforms.TransformUtils import processPreExec, processPreInclude, processPostExec, processPostInclude
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from AthenaConfiguration.ComponentFactory import CompFactory

#def defaultDerivationFlags(configFlags):
#   """Fill default derivation flags"""
#    # Not sure if anything will be needed here, leaving as a comment in case it is needed later

def fromRunArgs(runArgs):
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True

    from AthenaCommon.Logging import logging
    logDerivation = logging.getLogger('Derivation')
    logDerivation.info('****************** STARTING DERIVATION *****************')

    logDerivation.info('**** Transformation run arguments')
    logDerivation.info(str(runArgs))

    logDerivation.info('**** Setting-up configuration flags')
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    commonRunArgsToFlags(runArgs, ConfigFlags)

    # Switch on PerfMon
    ConfigFlags.PerfMon.doFullMonMT = True

    # Input types
    allowedInputTypes = [ 'AOD', 'DAOD_PHYS', 'EVNT' ]
    availableInputTypes = [ hasattr(runArgs, f'input{inputType}File') for inputType in allowedInputTypes ]
    if sum(availableInputTypes) != 1:
        raise ValueError('Input must be exactly one of the following types: inputAODFile, inputEVNTFile, inputDAOD_File')
    idx = availableInputTypes.index(True)
    ConfigFlags.Input.Files = getattr(runArgs, f'input{allowedInputTypes[idx]}File')

    # Output formats
    if hasattr(runArgs, 'formats'):
        logDerivation.info('Will attempt to make the following derived formats: {0}'.format(runArgs.formats))
        if 'PHYSVAL' in getattr(runArgs, 'formats'):
            ConfigFlags.BTagging.SaveSV1Probabilities = True
            ConfigFlags.BTagging.RunFlipTaggers = True
    else:
        logDerivation.error('Derivation job started, but with no output formats specified - aborting')
        raise ValueError('No derived formats specified')
    for runArg in dir(runArgs):    
        if 'output' in runArg:
            outputFileName = getattr(runArgs,runArg)
            flagString = 'Output.'+runArg.strip('output')+'Name'                   
            ConfigFlags.addFlag(flagString,outputFileName) 
            if not ConfigFlags.Output.doWriteDAOD:
                ConfigFlags.Output.doWriteDAOD = True

    # Pre-include
    processPreInclude(runArgs, ConfigFlags)

    # Pre-exec
    processPreExec(runArgs, ConfigFlags)

    # Lock flags
    ConfigFlags.lock()

    # The D(2)AOD building configuration 
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    # Pool file reading and writing
    cfg.merge(PoolReadCfg(ConfigFlags))
    cfg.merge(PoolWriteCfg(ConfigFlags))

    if hasattr(runArgs, 'formats'):
        for formatName in runArgs.formats:
            derivationConfig = getattr(DerivationConfigList, f'{formatName}Cfg')
            cfg.merge(derivationConfig(ConfigFlags))
    else:
        logDerivation.error('Derivation job started, but with no output formats specified - aborting')
        raise ValueError('No derived formats specified')


    # Pass-through mode (ignore skimming and accept all events)
    if hasattr(runArgs, 'passThrough'):    
        logDerivation.info('Pass-through mode was requested. All events will be written to the output.')        
        for algo in cfg.getEventAlgos():
            if isinstance(algo, CompFactory.DerivationFramework.DerivationKernel): 
                algo.SkimmingTools = []    

    # PerfMonSD 
    from PerfMonComps.PerfMonCompsConfig import PerfMonMTSvcCfg
    cfg.merge(PerfMonMTSvcCfg(ConfigFlags))

    # Set EventPrintoutInterval to 100 events
    cfg.getService(cfg.getAppProps()["EventLoop"]).EventPrintoutInterval = 100

    # Post-include
    processPostInclude(runArgs, ConfigFlags, cfg)

    # Post-exec
    processPostExec(runArgs, ConfigFlags, cfg)

    # Run the final configuration
    sc = cfg.run()
    sys.exit(not sc.isSuccess())
