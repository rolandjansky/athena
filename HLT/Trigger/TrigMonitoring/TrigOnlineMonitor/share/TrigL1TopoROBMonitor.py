#**************************************************************
#
# TrigL1TopoROBMonitor.py
#
#==============================================================
from AthenaCommon.Constants import *
from AthenaCommon import CfgMgr
L1TopoROBMonitor = CfgMgr.TrigL1TopoROBMonitor("L1TopoROBMonitor")

#--------------------------------------------------------------
# Add the algorithm to the topSequence
# (in L2 it should be running after steering, so that the ROB
#  cache is already filled)
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += L1TopoROBMonitor
#L1TopoROBMonitor.OutputLevel=1

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#
#
#==============================================================
#
# End of TrigROBMonitor.py
#
#**************************************************************
