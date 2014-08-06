
#----------------------------------------------------------------------
# Global and trigger flags
#

if (not 'costD3PD_doL2' in dir()):
  costD3PD_doL2 = False

if (not 'costD3PD_doEF' in dir()):
  costD3PD_doEF = False

if (not 'costD3PD_doHLT' in dir()):
  costD3PD_doHLT = True

#BSRDOInput=['data_test.00212967.Single_Stream.daq.RAW._lb0291._Athena._0101.data']

if not ('BSRDOInput' in dir()):
    print 'ERROR! Please supply a BSRDOInput to run over'
    sys.exit(1)

if not ('EvtMax' in dir()):
    EvtMax=0

if ('setEvent' in dir()) and type(setEvent) == int:
    from AthenaCommon.AppMgr import theApp
    theApp.EvtMax = setEvent

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.isOnline = True
athenaCommonFlags.BSRDOInput=BSRDOInput


from RecExConfig.RecFlags import rec
rec.readRDO = False
rec.AutoConfiguration=['everything']


#----------------------------------------------------------------------
# Set services for reading BS files
#
include("ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py")

svcMgr.ByteStreamCnvSvc.InitCnvs += [ "EventInfo",
                                      "HLT::HLTResult"]

svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_L2",
                                                   "HLT::HLTResult/HLTResult_EF",
                                                   "HLT::HLTResult/HLTResult_HLT"]


#----------------------------------------------------------------------
# Set input files
#
if ('BSRDOInput' in dir()):
    svcMgr.ByteStreamInputSvc.FullFileName = BSRDOInput

if len(svcMgr.ByteStreamInputSvc.FullFileName) == 0:
    print 'ERROR! svcMgr.ByteStreamInputSvc.FullFileName is empty'
    sys.exit(1)
    
#----------------------------------------------------------------------
# Setup cost algorithm
#
from AthenaCommon.AlgSequence import AlgSequence
from TrigCostMonitor.TrigCostMonitorConfig import prepareCostRun, setupCostDebug


costRunString = " "
if ('costD3PD_doL2' in dir() and bool(costD3PD_doL2) == True): costRunString += "l2 "
if ('costD3PD_doEF' in dir() and bool(costD3PD_doEF) == True): costRunString += "ef "
if ('costD3PD_doHLT' in dir() and bool(costD3PD_doHLT) == True): costRunString += "hlt "

print "Setting up TrigCostRun algorithm: Running over levels - " + costRunString
topSeq = AlgSequence()
runAlg = prepareCostRun('TrigCostRun', costRunString)

#setDebug = 1

if ('setDebug' in dir()):
    setupCostDebug()
    runAlg.OutputLevel = DEBUG
    runAlg.navigation.OutputLevel = DEBUG
    
    for tool in runAlg.tools:
        tool.OutputLevel = DEBUG

    print runAlg
    StoreGateSvc = Service("StoreGateSvc")
    StoreGateSvc.Dump = False

topSeq += runAlg

from AthenaCommon.Include import include
include("TrigCostD3PDMaker/TrigCostD3PDMaker_prodJobOFragment.py")

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
