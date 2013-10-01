###############################################################
#
# Job options file
#
#==============================================================

from AthenaCommon.AppMgr import theApp

###############################
# Load perf service
###############################
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not 'doMonitoring' in dir():
    doMonitoring = True
    pass
from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.doMonitoring = doMonitoring

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir(): EVTMAX = 10
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.OutputLevel = ERROR

## schedule StoreGate CLIDs test
###################################

from StoreGateTests.Lib import PyClidsTestWriter
topSequence += PyClidsTestWriter()

import AthenaPoolCnvSvc.WriteAthenaPool
topSequence += CfgMgr.AthenaOutputStream(
    "stream_stl",
    WritingTool = "AthenaPoolOutputStreamTool",
    OutputFile = 'out_stl_containers.pool'
    )
topSequence.stream_stl.ItemList += [
    "EventInfo#*",
    "std::vector<int>#*",
    ## note lack of space 'unsignedint' !!
    ## FIXME: https://savannah.cern.ch/bugs/index.php?39528
    "std::vector<unsignedint>#*",
    ##
    "std::vector<float>#*",
    "std::vector<double>#*",
    ]
