from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as af
af.FilesInput = [
"/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/data16_13TeV.00297447.physics_Main.daq.RAW._lb0555._SFO-1._0001.data"
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

from AthenaMonitoring.AthenaMonitoringConf import ManagedMonitorToolTest
TestTool = ManagedMonitorToolTest (
name           = "TestTool",
PreScale       = 0,
GeneratorSeed  = 65539,
EnableLumi = False,
ProcessNEvents = 40
## The next line is for cases when the AlgTool cannot be attached directly to AthenaMonManager
# ManagerName   = "AthMonTestMan"
)
TestTool.OutputLevel = INFO

ToolSvc += TestTool
monMan.AthenaMonTools += [ TestTool ]



