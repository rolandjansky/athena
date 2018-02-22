#**************************************************************
#
# TrigL1TopoWriteValData.py
#
#==============================================================

from AthenaCommon.Logging import logging
log = logging.getLogger('TrigL1TopoWriteValData.py')

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Check whether simulation algorithm provinding the input is in the sequence
topSequenceAlgNames=[alg.getName() for alg in topSequence.getChildren()]
if 'L1TopoSimulation' not in topSequenceAlgNames:
    log.error("missing L1TopoSimulation() from topSequence, cannot use TrigL1TopoWriteValData")
else:
    log.info("L1TopoSimulation() exists in topSequence: ok")
    log.debug("topSequence: %s" % topSequenceAlgNames)

# Add an instance of TrigL1TopoWriteValData to the sequence
from AthenaCommon import CfgMgr
l1topoWriteValData = CfgMgr.TrigL1TopoWriteValData("l1topoWriteValData")
topSequence += l1topoWriteValData
log.debug("added l1topoWriteValData to topSequence")
#l1topoWriteValData.OutputLevel=1

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#
#
#==============================================================
#
# End of TrigL1TopoWriteValData.py
#
#**************************************************************
