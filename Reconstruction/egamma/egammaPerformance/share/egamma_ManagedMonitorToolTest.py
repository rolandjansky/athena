from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as af
af.FilesInput = [
"/afs/cern.ch/user/y/yuraic/testarea/test_lumi/data/data12_8TeV.00211670.express_express.merge.ESD.r4065_p1278_tid01070414_00/ESD.01070414._000034.pool.root.1"
#"root://eosatlas.cern.ch//eos/atlas/atlasdatadisk/data12_8TeV/ESD/r4065/data12_8TeV.00204955.express_express.recon.ESD.r4065_tid01034439_00/ESD.01034439._000461.pool.root.1"
]

af.EvtMax=150 # number of event to process

from RecExConfig.RecFlags import rec
rec.AutoConfiguration = ['everything']
rec.readRDO = False
rec.readESD = True
rec.readAOD = True
rec.doCBNT = False
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
rec.doDPD = False
rec.doAOD = True
#rec.doDPD = True
rec.doFileMetaData = True

# Output log setting
OutputLevel = INFO

# main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")

# Must do tool configuration here for DB access to be autoconfigured from RecExCommon
from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
lumiTool = LuminosityToolDefault()
lumiTool.OutputLevel = DEBUG
ToolSvc += lumiTool

from LumiBlockComps.TrigLivefractionToolDefault import TrigLivefractionToolDefault
liveTool = TrigLivefractionToolDefault()
liveTool.OutputLevel = DEBUG
ToolSvc += liveTool
        
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## add an AthenaMonManager algorithm to the list of algorithms to be ran
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "AthMonTestMan" )

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.OutputLevel = INFO
#svcMgr.MessageSvc.OutputLevel = WARNING
#svcMgr.MessageSvc.OutputLevel = DEBUG

# ************************
# Monitoring configuration
# ************************

## Setup the output file(s):
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()

## The string "TestMon" in the argument below is the 'FileKey'
## used by Athena to access the output file internally
svcMgr.THistSvc.Output += ["TestMon DATAFILE='TestMon.root' OPT='RECREATE'"]
svcMgr.THistSvc.PrintAll = True
svcMgr.THistSvc.OutputLevel = 1

## AthenaMonManager is the Algorithm that manages many classes inheriting
## from ManagedMonitorToolBase
monMan = topSequence.AthMonTestMan

## FileKey must match that given to THistSvc
monMan.FileKey = "TestMon"

## Set global monitoring parameters: see the AthenaMonManager class
## in the Control/AthenaMonitoring package
monMan.ManualDataTypeSetup = True
monMan.DataType            = "monteCarlo"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = False
#monMan.Run                 = 1
#monMan.LumiBlock           = 1

#from AthenaMonitoring.AthenaMonitoringConf import ManagedMonitorToolTest
#TestTool = ManagedMonitorToolTest (
#name           = "TestTool",
#PreScale       = 0,
#GeneratorSeed  = 65539,
#EnableLumi = False,
#ProcessNEvents = 40
### The next line is for cases when the AlgTool cannot be attached directly to AthenaMonManager
## ManagerName   = "AthMonTestMan"
#)
from egammaPerformance.egammaPerformanceConf import egammaMonShifter
TestTool = egammaMonShifter (
name           = "egammaMonShifter",
PreScale       = 0,
EnableLumi = False,
ProcessNEvents = 40
## The next line is for cases when the AlgTool cannot be attached directly to AthenaMonManager
# ManagerName   = "AthMonTestMan"
)
TestTool.OutputLevel = INFO

ToolSvc += TestTool
monMan.AthenaMonTools += [ TestTool ]



