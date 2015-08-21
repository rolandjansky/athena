OutputName = "myHIST"

doJEPJEMMon = True
doJEPCMXMon = True
doCPMon = True
doCPSimMon = True
doJEPSimMon = True
doRODMon = True
doOverviewMon = True

FilesInput = [
    "root://eosatlas//eos/atlas/atlastier0/rucio/data15_13TeV/express_express/00270448/data15_13TeV.00270448.express_express.merge.RAW/data15_13TeV.00270448.express_express.merge.RAW._lb0007._SFO-ALL._0001.1",
]


from ByteStreamCnvSvc import ReadByteStream
svcMgr.ByteStreamInputSvc.FullFileName = FilesInput

include("TrigT1CaloByteStream/ReadLVL1CaloBSRun2_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# add an AthenaMonManager algorithm to the list of algorithms to be ran
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager("AthMonTestMan")

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"


# ************************
# Monitoring configuration
# ************************

# Setup the output file(s):
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()

# The string "TestMon" in the argument below is the 'FileKey'
# used by Athena to access the output file internally
svcMgr.THistSvc.Output += [
    "{0} DATAFILE='{0}.root' OPT='RECREATE'".format(OutputName)]
svcMgr.THistSvc.PrintAll = True
# svcMgr.THistSvc.OutputLevel = 1

# AthenaMonManager is the Algorithm that manages many classes inheriting
# from ManagedMonitorToolBase
L1CaloMan = topSequence.AthMonTestMan

# FileKey must match that given to THistSvc
L1CaloMan.FileKey = OutputName

# Set global monitoring parameters: see the AthenaL1CaloManager class
# in the Control/AthenaMonitoring package
L1CaloMan.ManualDataTypeSetup = True
L1CaloMan.DataType = "data"
L1CaloMan.Environment = "altprod"
L1CaloMan.ManualRunLBSetup = False
#monMan.Run                 = 1
#monMan.LumiBlock           = 1

if doJEPJEMMon:
    from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__JEPJEMMon
    L1JEPJEMMonTool = LVL1__JEPJEMMon("L1JEPJEMMonTool")
    ToolSvc += L1JEPJEMMonTool
    L1CaloMan.AthenaMonTools += [L1JEPJEMMonTool]

if doJEPCMXMon:
    from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__JEPCMXMon
    L1JEPCMXMonTool = LVL1__JEPCMXMon("L1JEPCMXMonTool")
    ToolSvc += L1JEPCMXMonTool
    L1CaloMan.AthenaMonTools += [L1JEPCMXMonTool]

if doCPMon:
    from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__CPMon
    L1CPMMonTool = LVL1__CPMon("L1CPMMonTool")
    ToolSvc += L1CPMMonTool
    L1CaloMan.AthenaMonTools += [L1CPMMonTool]


if doCPSimMon:
    from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__CPSimMon
    CPSimMonTool = LVL1__CPSimMon("CPSimMonTool")
    ToolSvc += CPSimMonTool
    L1CaloMan.AthenaMonTools += [CPSimMonTool]

if doJEPSimMon:
    from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__JEPSimMon
    JEPSimMonTool = LVL1__JEPSimMon("JEPSimMonTool")
    ToolSvc += JEPSimMonTool
    L1CaloMan.AthenaMonTools += [JEPSimMonTool]

if doRODMon:
    from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__RODMon
    L1BSRODMonTool = LVL1__RODMon(
        name="L1BSRODMonTool",
    )
    ToolSvc += L1BSRODMonTool
    L1CaloMan.AthenaMonTools += [L1BSRODMonTool]


if doOverviewMon:
    from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__OverviewMon
    L1OverviewMonTool = LVL1__OverviewMon("L1OverviewMonTool")
    ToolSvc += L1OverviewMonTool
    L1CaloMan.AthenaMonTools += [L1OverviewMonTool]

    from TrigT1CaloMonitoringTools.TrigT1CaloMonitoringToolsConf import LVL1__TrigT1CaloMonErrorTool
    L1MonErrorTool = LVL1__TrigT1CaloMonErrorTool(
        name="TrigT1CaloMonErrorTool",
        FlagCorruptEvents="None"
    )
    ToolSvc += L1MonErrorTool

theApp.EvtMax = 50
