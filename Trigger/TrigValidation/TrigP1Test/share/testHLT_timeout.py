#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

# This file is based on MTCalibPeb.py

from AthenaCommon.Logging import logging
log = logging.getLogger('testHLT_timeout.py')

# SGInputLoader takes care of unmet input dependencies (e.g. triggering conversion from BS)
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.setDataLoaderAlg('SGInputLoader')

# Configure the HLT algorithms
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior += 1

import TrigExPartialEB.MTCalibPebConfig as Conf
hypo_tools = [Conf.make_hypo_tool('HLT_MTCalibPeb{:d}'.format(num)) for num in range(1, 4)]
for tool in hypo_tools:
    # 100% accept rate, no ROB requests, sleeps for up to 1.4 seconds
    tool.RandomAcceptRate = 1.0
    tool.ROBAccessDict = {}
    tool.BurnTimePerCycleMillisec = 200
    tool.NumBurnCycles = 7
    tool.PEBROBList = [0x7c0000]

hypo = Conf.make_hypo_alg('HypoAlg1')
hypo.HypoTools = hypo_tools

Configurable.configurableRun3Behavior -= 1

# Set flags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
Conf.set_flags(ConfigFlags)

# Configure the L1 and HLT sequences
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
CAtoGlobalWrapper(Conf.l1_seq_cfg, ConfigFlags)
CAtoGlobalWrapper(Conf.hlt_seq_cfg, ConfigFlags,
                  num_chains=1,  # ignored if hypo_algs argument given
                  concurrent=False,  # ignored if hypo_algs argument given
                  hackCA2Global=True,
                  hypo_algs=[hypo])

# Print configuration for debugging
log.info('Dump of topSequence')
from AthenaCommon.AlgSequence import AlgSequence, dumpSequence
topSequence = AlgSequence()
dumpSequence(topSequence)
log.info('Dump of ServiceMgr')
from AthenaCommon.AppMgr import ServiceMgr
dumpSequence(ServiceMgr)
