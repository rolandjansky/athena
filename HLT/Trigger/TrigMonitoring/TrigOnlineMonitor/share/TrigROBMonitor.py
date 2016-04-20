#**************************************************************
#
# TrigROBMonitor.py
#
#==============================================================
from AthenaCommon.Constants import *
from AthenaCommon import CfgMgr
ROBMonitor = CfgMgr.TrigROBMonitor("ROBMonitor")

#--------------------------------------------------------------
# Add the algorithm to the topSequence
# (in L2 it should be running after steering, so that the ROB
#  cache is already filled)
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += ROBMonitor

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#
# --- do ROB checksum test
#
#ROBMonitor.TestROBChecksum            = True
#
# --- Histogram definitions for ROB checksum test
#
#ROBMonitor.HistFailedChecksumForROB   = ('FailedChecksumForROB', 0, 50, 50)
#ROBMonitor.HistFailedChecksumForSD    = ('FailedChecksumForSubDetector', 0, 60, 60)
#
# --- put events with checksum failures on debug stream
#
#ROBMonitor.SetDebugStream             = True
#
# --- debug stream name for events with checksum failures
#
#ROBMonitor.DebugStreamName            = 'RobErrorStream'
#
# --- plot generic/specific status bits for ROB fragments
#
#ROBMonitor.TestROBStatus              = True
#
# --- plot ROB data volume
#
#ROBMonitor.PlotROBDataVolume          = True
#
# --- Histogram definitions for ROB data volume
#
#ROBMonitor.HistTotalDataVolumeROB     = ('totalDataVolumeForROBs', 0., 5000., 50)
#
#==============================================================
#
# End of TrigROBMonitor.py
#
#**************************************************************
