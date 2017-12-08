from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as af
af.FilesInput = [
"/afs/cern.ch/work/d/derendar/data15_13TeV.00267359.physics_MinBias.merge.AOD.r7111_p2424_tid06562979_00/AOD.06562979._000370.pool.root.1"
#"/afs/cern.ch/work/d/derendar/data15_13TeV.00271744.physics_Main.merge.AOD.r7111_p2424_tid06550034_00/AOD.06550034._000243.pool.root.1"
#"/afs/cern.ch/work/d/derendar/data11_hi.00193270.physics_MinBias.recon.AOD.r6991_tid06320830_00/AOD.06320830._004356.pool.root.1"
#"root://eosatlas//eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data11_hi.00193403.physics_HardProbes.merge.RAW._lb0012._SFO-9._0001.1"
#"/afs/cern.ch/work/d/derendar/mc15_5TeV.420000.Hijing_PbPb_5p02TeV_MinBias_Flow_JJFV6.recon.AOD.e3754_s2633_r6797_tid05633705_00/AOD.05633705._000215.pool.root.1"
]

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

if DQMonFlags.useTrigger():
        if not hasattr(ToolSvc, DQMonFlags.nameTrigDecTool()):
                include("AthenaMonitoring/TrigDecTool_jobOptions.py")

from HIMonitoring.HIMonitoringConf import HIMonitoringEventShapeTool
TestTool = HIMonitoringEventShapeTool (
name           = "TestTool",
EnableLumi = False,
ZDCmon = True, 
ESmon = True, 
)
TestTool.OutputLevel = INFO

## HLT chains expected to be in the ex stream
TestTool.TriggerChain = "HLT_noalg_mb_L1ZDC_A_C,HLT_noalg_mb_L1MBTS_1_1,HLT_noalg_mb_L1TE5"
if DQMonFlags.useTrigger():
    TestTool.TrigDecisionTool = monTrigDecTool #tool should already be in namespace

ToolSvc += TestTool
monMan.AthenaMonTools += [ TestTool ]


## Photons 
from HIMonitoring.HIMonitoringConf import HIMonitoringPhotonsTool
PhotonsTool = HIMonitoringPhotonsTool (
name           = "PhotonsTool",
)
PhotonsTool.OutputLevel = INFO


PhotonsTool.TriggerChain = "HLT_g15_loose"
if DQMonFlags.useTrigger():
    PhotonsTool.TrigDecisionTool = monTrigDecTool #tool should already be in namespace

ToolSvc += PhotonsTool
monMan.AthenaMonTools += [ PhotonsTool ]

## ZDC 
from HIMonitoring.HIMonitoringConf import HIMonitoringZdcTool
ZDCTool = HIMonitoringZdcTool (
name           = "ZDCTool",
)
ZDCTool.OutputLevel = INFO

# if DQMonFlags.useTrigger():
    # ZDCTool.TrigDecisionTool = monTrigDecTool #tool should already be in namespace

ToolSvc += ZDCTool
monMan.AthenaMonTools += [ ZDCTool ]


## Muons 
from HIMonitoring.HIMonitoringConf import HIMonitoringMuonsTool
MuonsTool = HIMonitoringMuonsTool (
name           = "MuonsTool",
)
MuonsTool.OutputLevel = INFO

MuonsTool.TriggerChain = "HLT_mu4,HLT_mu10,HLT_mu14"
if DQMonFlags.useTrigger():
    MuonsTool.TrigDecisionTool = monTrigDecTool #tool should already be in namespace

ToolSvc += MuonsTool
monMan.AthenaMonTools += [ MuonsTool ]

## Electrons 
from HIMonitoring.HIMonitoringConf import HIMonitoringElectronsTool
ElectronsTool = HIMonitoringElectronsTool (
name           = "ElectronsTool",
)
ElectronsTool.OutputLevel = INFO

# ElectronsTool.TriggerChain = ""
# if DQMonFlags.useTrigger():
    # ElectronsTool.TrigDecisionTool = monTrigDecTool #tool should already be in namespace

ToolSvc += ElectronsTool
monMan.AthenaMonTools += [ ElectronsTool ]
