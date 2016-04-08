###############################################################
#
# Job options for testing adding bins on the fly while
# publication is ongoing
#
# Run this e.g. with:
#   athenaMT -M -f athenaMT.data -n100000 TrigExMTHistNtup/MTTestBinning.py
#
#==============================================================
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
from TrigExMTHistNtup.TrigExMTHistNtupConf import MTTHist
topSequence = AlgSequence()

topSequence += MTTHist(OutputLevel = INFO,
                       addBinEveryNEvents = 10,
                       nHistsForBinTest = 50,
                       useMutex = True
                       )
