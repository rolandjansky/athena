
#----------------------------------------------------------------------
# Global and trigger flags
#

if (not 'costD3PD_doL2' in dir()):
  costD3PD_doL2 = False

if (not 'costD3PD_doEF' in dir()):
  costD3PD_doEF = False

if (not 'costD3PD_doHLT' in dir()):
  costD3PD_doHLT = True

if (not 'costOutput' in dir()):
  costOutput = "COST RATE EB"

#BSRDOInput=['data_test.00212967.Single_Stream.daq.RAW._lb0291._Athena._0101.data']

if not ('BSRDOInput' in dir()):
    print 'ERROR! Please supply a BSRDOInput to run over'
    sys.exit(1)

if not ('EvtMax' in dir()):
    EvtMax=-1
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = EvtMax

if ('setEvent' in dir()) and type(setEvent) == int:
    from AthenaCommon.AppMgr import theApp
    theApp.setEvent = setEvent

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.isOnline = True
athenaCommonFlags.BSRDOInput=BSRDOInput


from RecExConfig.RecFlags import rec
rec.readRDO = False
rec.AutoConfiguration=['everything']

from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

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
    svcMgr.EventSelector.Input = BSRDOInput

if len(svcMgr.EventSelector.Input) == 0:
    print 'ERROR! svcMgr.EventSelector.Input is empty'
    sys.exit(1)

#----------------------------------------------------------------------
# Setup cost algorithm
#
from TrigCostMonitor.TrigCostMonitorConfig import prepareCostRun, setupCostDebug

costRunString = " "
if ('costD3PD_doL2' in dir() and bool(costD3PD_doL2) == True): costRunString += "l2 "
if ('costD3PD_doEF' in dir() and bool(costD3PD_doEF) == True): costRunString += "ef "
if ('costD3PD_doHLT' in dir() and bool(costD3PD_doHLT) == True): costRunString += "hlt "

print "Setting up TrigCostRun algorithm: Running over levels - " + costRunString
runAlg = prepareCostRun('TrigCostRun', costRunString)

#setDebug = 1

if ('enableCostDebug' in dir() and bool(enableCostDebug) == True):
    print 'Setting TrigCostRun to DEBUG output level'
    setupCostDebug()
    runAlg.OutputLevel = DEBUG
    runAlg.navigation.OutputLevel = DEBUG
    for tool in runAlg.tools:
        tool.OutputLevel = DEBUG
    print runAlg

topSeq += runAlg

from AthenaCommon.Include import include
if ("COST" in costOutput):
  include("TrigCostD3PDMaker/TrigCostD3PDMaker_prodJobOFragment.py")

if ("RATE" in costOutput):
  include("TrigCostD3PDMaker/TrigRateD3PDMaker_prodJobOFragment.py")

if ("EB" in costOutput):
  include("TrigCostD3PDMaker/TrigEBWeightD3PDMaker_prodJobOFragment.py")

#----------------------------------------------------------------------
# Message service and loop manager options
#
ServiceMgr.MessageSvc.OutputLevel = 3
#ServiceMgr.MessageSvc.defaultLimit = 0
#ServiceMgr.MessageSvc.enableSuppression = False
ServiceMgr.MessageSvc.Format = "% F%35W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.showStats = True
ServiceMgr.MessageSvc.statLevel = WARNING

if not hasattr(ServiceMgr, "AthenaEventLoopMgr"):
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    ServiceMgr += AthenaEventLoopMgr()
    
ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100

