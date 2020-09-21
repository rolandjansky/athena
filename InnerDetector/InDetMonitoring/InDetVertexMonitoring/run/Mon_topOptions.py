# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as af
af.FilesInput = [
  "/afs/cern.ch/user/f/fmeloni/conf_file/AOD.541757._000001.pool.root.1"
]
af.EvtMax=10000 # number of event to process

#======================================================================================
# *************************
# Application configuration
# *************************

#import AthenaCommon.AtlasUnixGeneratorJob

## get the one and only one Athena Application Mgr
from AthenaCommon.AppMgr import theApp
#theApp.EvtMax  = 100
## get a handle on the top sequence of algorithms
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from RecExConfig.RecFlags import rec
rec.AutoConfiguration = ['everything']
rec.readRDO = False
rec.readESD = False
rec.readAOD = True
rec.doCBNT = False
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False
rec.doDPD = True
rec.doFileMetaData = True

# Output log setting
OutputLevel = INFO

# main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")

## add an AthenaMonManager algorithm to the list of algorithms to be ran
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
topSequence += AthenaMonManager( "AthMonTestMan" )

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.MessageSvc.OutputLevel = ERROR

# ************************
# Monitoring configuration
# ************************

## Setup the output file(s):
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()

## The string "TestMon" in the argument below is the 'FileKey'
## used by Athena to access the output file internally
svcMgr.THistSvc.Output += ["TestMon DATAFILE='VxMon.root' OPT='RECREATE'"]

## AthenaMonManager is the Algorithm that manages many classes inheriting
## from ManagedMonitorToolBase
monMan = topSequence.AthMonTestMan

## FileKey must match that given to THistSvc
monMan.FileKey = "TestMon"

## Set global monitoring parameters: see the AthenaMonManager class
## in the Control/AthenaMonitoring package
#monMan.ManualDataTypeSetup = True
monMan.DataType            = "collisions"
monMan.Environment         = "tier0"
#monMan.ManualRunLBSetup    = True
#monMan.Run                 = 1
#monMan.LumiBlock           = 1

# list of the algorithms to be executed at run time
from InDetVertexMonitoring.InDetVertexMonitoringConf import InDetVertexMonitoring
myVx = InDetVertexMonitoring()
myVx.DoControlPlots = False
myVx.OutputLevel = WARNING

if not 'DQMonFlags' in dir():
    print("DQMonFlags not yet imported - I import them now")
    from AthenaMonitoring.DQMonFlags import DQMonFlags

#DQMonFlags.useTrigger = False 

if DQMonFlags.useTrigger():
   if not 'rec' in dir():
      from RecExConfig.RecFlags import rec

   if rec.readESD() and (DQMonFlags.monManEnvironment=='tier0ESD'):
      # for ESD->AOD use trigger config stored as in-file meta-data,
      if not 'TriggerFlags' in dir():
         from TriggerJobOpts.TriggerFlags import TriggerFlags
      TriggerFlags.configurationSourceList = ['ds']

      # set up trigger config service
      if not 'TriggerConfigGetter' in dir():
         from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
         cfg = TriggerConfigGetter()

   if not hasattr(ToolSvc, DQMonFlags.nameTrigDecTool()):
      from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
      monTrigDecTool = Trig__TrigDecisionTool(name=DQMonFlags.nameTrigDecTool(),
        				      OutputLevel=ERROR,
        				      PublicChainGroups = {"EF": "EF_rd0*",}
        				     )
   ToolSvc += monTrigDecTool

if DQMonFlags.useTrigger():
  if not hasattr(ToolSvc, 'monTrigDecTool'):
    print "Trigger decision tool not found: don't run trigger-aware monitoring"  
  else:
    myVx.TrigDecisionTool = monTrigDecTool #tool should already be in namespace
    myVx.TriggerChain = "EF_rd0_filled_NoAlg"

ToolSvc += myVx
monMan.AthenaMonTools += [ myVx ]


