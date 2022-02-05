#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from TrigExamples import MTCalibPebConfig
from AthenaCommon.Logging import logging
log = logging.getLogger('MTCalibPeb.py')

def get_opt(name, default):
    return globals()[name] if name in globals() else default

# Options which can be set by running with -c 'optname=value'
# Flag controlling if MTCalibPeb chains are concurrent or sequential,
concurrent = get_opt('concurrent', False)
# Number of chains
num_chains = get_opt('num_chains', 3)
# Configure hypo tool options
MTCalibPebConfig.default_options.UseRandomSeed = get_opt('UseRandomSeed', False)
MTCalibPebConfig.default_options.RandomAcceptRate = get_opt('RandomAcceptRate', 0.01)
MTCalibPebConfig.default_options.BurnTimePerCycleMillisec = get_opt('BurnTimePerCycleMillisec', 20)
MTCalibPebConfig.default_options.NumBurnCycles = get_opt('NumBurnCycles', 10)
MTCalibPebConfig.default_options.BurnTimeRandomly = get_opt('BurnTimeRandomly', True)
MTCalibPebConfig.default_options.Crunch = get_opt('Crunch', False)
MTCalibPebConfig.default_options.CheckDataConsistency = get_opt('CheckDataConsistency', False)
MTCalibPebConfig.default_options.ROBAccessDict = get_opt('ROBAccessDict', MTCalibPebConfig.rob_access_dict)
MTCalibPebConfig.default_options.TimeBetweenROBReqMillisec = get_opt('TimeBetweenROBReqMillisec', 0)
MTCalibPebConfig.default_options.PEBROBList = get_opt('PEBROBList', [])
MTCalibPebConfig.default_options.PEBSubDetList = get_opt('PEBSubDetList', [])
MTCalibPebConfig.default_options.CreateRandomData = get_opt('CreateRandomData', {})
MTCalibPebConfig.default_options.EnableL1CaloPhase1 = get_opt('EnableL1CaloPhase1', False)
MTCalibPebConfig.default_options.EnableL1MuonPhase1 = get_opt('EnableL1MuonPhase1', False)
MTCalibPebConfig.default_options.EnableL1CaloLegacy = get_opt('EnableL1CaloLegacy', True)

# SGInputLoader takes care of unmet input dependencies (e.g. triggering conversion from BS)
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.setDataLoaderAlg('SGInputLoader')

# Increase scheduler checks and verbosity
AlgScheduler.CheckDependencies( True )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )

# Set flags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
MTCalibPebConfig.set_flags(ConfigFlags)
ConfigFlags.lock()

# Configure the L1 and HLT sequences
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
CAtoGlobalWrapper(MTCalibPebConfig.l1_seq_cfg, ConfigFlags)
CAtoGlobalWrapper(MTCalibPebConfig.hlt_seq_cfg, ConfigFlags,
                  num_chains=num_chains, concurrent=concurrent, hackCA2Global=True)

# Print configuration for debugging
log.info('Dump of topSequence')
from AthenaCommon.AlgSequence import AlgSequence, dumpSequence
topSequence = AlgSequence()
dumpSequence(topSequence)
log.info('Dump of ServiceMgr')
from AthenaCommon.AppMgr import ServiceMgr
dumpSequence(ServiceMgr)
