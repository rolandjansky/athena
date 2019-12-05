#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from TrigExPartialEB import MTCalibPebConfig
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
MTCalibPebConfig.default_options.RandomAcceptRate = get_opt('RandomAcceptRate', 0.01)
MTCalibPebConfig.default_options.BurnTimePerCycleMillisec = get_opt('BurnTimePerCycleMillisec', 20)
MTCalibPebConfig.default_options.NumBurnCycles = get_opt('NumBurnCycles', 10)
MTCalibPebConfig.default_options.BurnTimeRandomly = get_opt('BurnTimeRandomly', True)
MTCalibPebConfig.default_options.Crunch = get_opt('Crunch', False)
MTCalibPebConfig.default_options.ROBAccessDict = get_opt('ROBAccessDict', MTCalibPebConfig.rob_access_dict)
MTCalibPebConfig.default_options.TimeBetweenROBReqMillisec = get_opt('TimeBetweenROBReqMillisec', 0)
MTCalibPebConfig.default_options.PEBROBList = get_opt('PEBROBList', [])
MTCalibPebConfig.default_options.PEBSubDetList = get_opt('PEBSubDetList', [])
MTCalibPebConfig.default_options.CreateRandomData = get_opt('CreateRandomData', {})

# SGInputLoader takes care of unmet input dependencies (e.g. triggering conversion from BS)
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.setDataLoaderAlg('SGInputLoader')

# The top algo sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += MTCalibPebConfig.make_l1_seq()
topSequence += MTCalibPebConfig.make_hlt_seq(num_chains, concurrent)

# Print configuration for debugging
log.info('Dump of topSequence')
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)
log.info('Dump of ServiceMgr')
from AthenaCommon.AppMgr import ServiceMgr
dumpSequence(ServiceMgr)
