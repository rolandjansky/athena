
#----------------------------------------------------------------------
# Global and trigger flags
#
if not ('EvtMax' in dir()):
    EvtMax=0

if ('setEvent' in dir()) and type(setEvent) == int:
    from AthenaCommon.AppMgr import theApp
    theApp.EvtMax = setEvent

from AthenaCommon.GlobalFlags import globalflags
globalflags.InputFormat = 'bytestream'
globalflags.DetGeo      = 'atlas'
globalflags.DataSource  = 'data'

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.readBS = True
TriggerFlags.doLVL1 = False
TriggerFlags.doLVL2 = False
TriggerFlags.doEF   = False

#----------------------------------------------------------------------
# Set services for reading BS files
#
include("ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py")

svcMgr.ByteStreamCnvSvc.InitCnvs += [ "EventInfo",
                                      "HLT::HLTResult" ]

svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_L2",
                                                   "HLT::HLTResult/HLTResult_EF" ]

#----------------------------------------------------------------------
# Set input files
#
if not ('BSRDOInput' in dir()) or len(BSRDOInput) < 1:
    from TrigCostMonitor.TrigCostMonitorConfig import readInputFiles
    BSRDOInput = readInputFiles('input_files.txt', 'RAW')
    
if ('BSRDOInput' in dir()):
    svcMgr.EventSelector.Input = BSRDOInput

if len(svcMgr.EventSelector.Input) == 0:
    print 'ERROR! svcMgr.EventSelector.Input is empty'
    sys.exit(1)
    
#----------------------------------------------------------------------
# Setup cost algorithm
#
from AthenaCommon.AlgSequence import AlgSequence
from TrigCostMonitor.TrigCostMonitorConfig import prepareReadOnlineAlg

topSeq = AlgSequence()
runAlg = prepareReadOnlineAlg('TrigCostAlg', 'read')

if ('enableCostDebug' in dir()) and bool(enableCostDebug) == True:
    print 'enableCostDebug = TRUE. Adding extra debug output'
    runAlg.OutputLevel = 2
    for tool in runAlg.tools:
        tool.OutputLevel = 2
    print runAlg
    StoreGateSvc = Service("StoreGateSvc")
    StoreGateSvc.Dump = True

topSeq += runAlg

#----------------------------------------------------------------------
# Message service and loop manager options
#
ServiceMgr.MessageSvc.OutputLevel = 3
ServiceMgr.MessageSvc.defaultLimit = 0
ServiceMgr.MessageSvc.enableSuppression = False
ServiceMgr.MessageSvc.Format = "% F%35W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.showStats = True
ServiceMgr.MessageSvc.statLevel = WARNING

if not hasattr(ServiceMgr, "AthenaEventLoopMgr"):
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    ServiceMgr += AthenaEventLoopMgr()
    
ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100
