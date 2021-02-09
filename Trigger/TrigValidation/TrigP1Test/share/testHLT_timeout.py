#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

# This file is based on MTCalibPeb.py

from AthenaCommon.Logging import logging
log = logging.getLogger('testHLT_timeout.py')

# SGInputLoader takes care of unmet input dependencies (e.g. triggering conversion from BS)
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.setDataLoaderAlg('SGInputLoader')

# Configure the HLT algorithms
import TrigExPartialEB.MTCalibPebConfig as Conf
hypo_tools = [Conf.make_hypo_tool('HLT_MTCalibPeb{:d}'.format(num)) for num in range(1, 4)]
for tool in hypo_tools:
    # 100% accept rate, no ROB requests, sleeps for up to 1.4 seconds
    tool.RandomAcceptRate = 1.0
    tool.ROBAccessDict = []
    tool.BurnTimePerCycleMillisec = 200
    tool.NumBurnCycles = 7
    tool.PEBROBList = [0x7c0000]

hypo = Conf.make_hypo_alg('HypoAlg1')
hypo.HypoTools = hypo_tools
Conf.configure_hlt_result([hypo])
summary_algs = Conf.make_summary_algs([hypo])

# Create the L1 sequence and add to topSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += Conf.make_l1_seq()

# Create the HLT sequence and add to topSequence
from AthenaCommon.CFElements import seqOR
all_algs = [hypo]
all_algs.extend(summary_algs)
topSequence += seqOR('hltTop', all_algs)

# Print configuration for debugging
log.info('Dump of topSequence')
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)
log.info('Dump of ServiceMgr')
from AthenaCommon.AppMgr import ServiceMgr
dumpSequence(ServiceMgr)
