from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'

# # for AFP calibration stream 13 TeV physics run
# globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')
# # from AthenaCommon.JobProperties import jobproperties
# # JobProperties.Global.DetDescrVersion

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as af
af.FilesInput = [
#"/media/DrugiDysk/ggach/ATLAS/AFPMonitoring/data/xAOD.out.root"
#"/media/DrugiDysk/ggach/ATLAS/AFPxAOD/data/moreData.root"
#"/media/DrugiDysk/ggach/ATLAS/AFPxAOD/data/xAOD.out.root"
#"/media/DrugiDysk/ggach/ATLAS/AFPxAOD/data/testxAOD.root"
#"/media/DrugiDysk/ggach/ATLAS/AFPxAOD/data/testxAOD2.root"
#"/media/DrugiDysk/ggach/ATLAS/AFPxAOD/data/testxAOD305359_1.root"
#"/media/DrugiDysk/ggach/ATLAS/AFPxAOD/data/testxAOD305359_2.root"
#"/media/DrugiDysk/ggach/ATLAS/AFPxAOD/data/testxAOD305359_3.root"
"/media/DrugiDysk/ggach/ATLAS/AFPxAOD/data/testxAOD305359_4.root"
]

#af.EvtMax=1000 # number of event to process
af.EvtMax=-1 # number of event to process

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
OutputLevel = WARNING

# main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")

# Must do tool configuration here for DB access to be autoconfigured from RecExCommon
from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault
lumiTool = LuminosityToolDefault()
lumiTool.OutputLevel = WARNING
ToolSvc += lumiTool

# from LumiBlockComps.TrigLivefractionToolDefault import TrigLivefractionToolDefault
# liveTool = TrigLivefractionToolDefault()
# liveTool.OutputLevel = DEBUG
# ToolSvc += liveTool
        
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## add an AthenaMonManager algorithm to the list of algorithms to be ran
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "AthMonTestMan" )

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.OutputLevel = WARNING
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
svcMgr.THistSvc.OutputLevel = 4

## AthenaMonManager is the Algorithm that manages many classes inheriting
## from ManagedMonitorToolBase
monMan = topSequence.AthMonTestMan

## FileKey must match that given to THistSvc
monMan.FileKey = "TestMon"

## Set global monitoring parameters: see the AthenaMonManager class
## in the Control/AthenaMonitoring package
monMan.ManualDataTypeSetup = True
#monMan.DataType            = "monteCarlo"
monMan.DataType            = "data"
monMan.Environment         = "altprod"
monMan.ManualRunLBSetup    = False
#monMan.Run                 = 1
#monMan.LumiBlock           = 1

from AFP_Monitoring.AFP_MonitoringConf import AFPHitsMonitorTool
TestTool = AFPHitsMonitorTool (
name           = "TestTool",
PreScale       = 0,
EnableLumi = False,
ProcessNEvents = 40
## The next line is for cases when the AlgTool cannot be attached directly to AthenaMonManager
# ManagerName   = "AthMonTestMan"
)
TestTool.OutputLevel = WARNING

ToolSvc += TestTool
monMan.AthenaMonTools += [ TestTool ]



