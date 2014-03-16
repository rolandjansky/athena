#**************************************************************
#
# PebFilterOptions.py
#
#==============================================================
from AthenaCommon.Constants import *
from AthenaCommon import CfgMgr

PebFilter = CfgMgr.TrigPartialEBFilter("PebFilter")

#--------------------------------------------------------------
# Add the algorithm to the topSequence
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += PebFilter

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#
# --- Filter physics events (50% of the accepted rate)
#
#PebFilter.AcceptRate       = .5
#
# --- Select a particular calibration stream for the building
#
#PebFilter.CalibrationStreamName   = "StreamName"
#
# --- Set specific tag for the HLT result 
#
#PebFilter.HLTInstance    = "L2" or "EF"
#PebFilter.HLTResultName  = "CalibrationStreamName"
#
#==============================================================
#
# End of PebFilterOptions.py
#
#**************************************************************
