###############################################################
#
# Job options file
#
#==============================================================

from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,WARNING,ERROR
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = -1
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

###############################
# Load perf service
###############################
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doDetailedMonitoring = True

################################################################
# Steer digitization job
#########################
from AthenaCommon.AthenaCommonFlags import jobproperties as jp
jp.AthenaCommonFlags.EvtMax= EVTMAX
jp.AthenaCommonFlags.PoolHitsInput=["rfio:/castor/cern.ch/user/s/svahsen/digitization/RTT/calib1_csc11.005200.T1_McAtNlo_Jimmy.simul.HITS.v12003104_tid004131._00069.pool.root.10"]
jp.AthenaCommonFlags.PoolRDOOutput="/tmp/calib1_csc11.005200.digits.pool"

import os
_poolRDOOutput = jp.AthenaCommonFlags.PoolRDOOutput()
if os.path.exists(_poolRDOOutput): os.remove(_poolRDOOutput)
del _poolRDOOutput

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties
# jobproperties.Digitization.doMinimumBias=True
# jobproperties.Digitization.numberOfCollisions=2.3
# jobproperties.Digitization.minBiasInputCols=["", "", "" ]
# jobproperties.Digitization.doCavern=True
# jobproperties.Digitization.cavernInputCols=["",""]

#--------------------------------------------------------------
# Set some of the global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-CSC-01-02-00'

# Flags that are defined in python are best set here
# switch off ID and muons
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOff()

include("Digitization/Digitization.py")
###############################################################

## setting up output list items
outStreams = AlgSequence("Streams")
outStreams.Stream1.ItemList = [
          "EventInfo#McEventInfo",
          "CaloCalibrationHitContainer#LArCalibrationHitActive",
          "CaloCalibrationHitContainer#LArCalibrationHitDeadMaterial",
          "CaloCalibrationHitContainer#LArCalibrationHitInactive",
          "CaloCalibrationHitContainer#TileCalibrationCellHitCnt",
          "CaloCalibrationHitContainer#TileCalibrationDMHitCnt",
          "LArRawChannelContainer#LArRawChannels",
          "McEventCollection#TruthEvent",
          "TileRawChannelContainer#TileRawChannelCnt",
##           "TileRawChannelCollectionVec#TileRawChannelFiltered",
          "TileHitVector#MBTSHits",
          "TrackRecordCollection#CaloEntryLayer",
          "TrackRecordCollection#MuonEntryLayer",
          "TrackRecordCollection#MuonExitLayer"
          ]

## repeat 10 times the same event
## jobproperties.PerfMonFlags.installEvtSelector = True
## svcMgr.PerfMonEventSelector.NbrReplays = 10-1
## svcMgr.AthenaPoolCnvSvc.CommitInterval = 10

#==============================================================
#
# End of job options file
#
###############################################################
