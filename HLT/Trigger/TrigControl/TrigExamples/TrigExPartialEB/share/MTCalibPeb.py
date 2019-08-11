#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from AthenaCommon.Logging import logging
log = logging.getLogger('MTCalibPeb.py')

# Flag controlling if MTCalibPeb chains are concurrent or sequential,
# can be set by running with -c 'concurrent=True'
if 'concurrent' in globals():
    concurrent = globals()['concurrent']
else:
    concurrent = False

# SGInputLoader takes care of unmet input dependencies (e.g. triggering conversion from BS)
from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.setDataLoaderAlg('SGInputLoader')

# The top algo sequence
from AthenaCommon.AlgSequence import AlgSequence
from TrigExPartialEB.MTCalibPebConfig import make_l1_seq, make_hlt_seq
topSequence = AlgSequence()
topSequence += make_l1_seq()
topSequence += make_hlt_seq(concurrent)

# Print configuration for debugging
log.info('Dump of topSequence')
from AthenaCommon.AlgSequence import dumpSequence
dumpSequence(topSequence)
log.info('Dump of ServiceMgr')
from AthenaCommon.AppMgr import ServiceMgr
dumpSequence(ServiceMgr)
