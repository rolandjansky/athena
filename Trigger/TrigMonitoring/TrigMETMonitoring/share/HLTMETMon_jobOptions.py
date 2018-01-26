#############################################################
# File   : Job Options to run monitoring in stand-alone mode 
# Author : Venkat.Kaushik@cern.ch
# Date   : Jan 2010
#############################################################
doLocal = True

import sys

##############
# Input files
##############

InputFiles = ['/hep300/data/khamano/data17/data17_13TeV.00331129.physics_Main.merge.AOD.f848_m1844/data17_13TeV.00331129.physics_Main.merge.AOD.f848_m1844._lb0285._0002.1','/hep300/data/khamano/data17/data17_13TeV.00331129.physics_Main.merge.AOD.f848_m1844/data17_13TeV.00331129.physics_Main.merge.AOD.f848_m1844._lb0288._0004.1','/hep300/data/khamano/data17/data17_13TeV.00331129.physics_Main.merge.AOD.f848_m1844/data17_13TeV.00331129.physics_Main.merge.AOD.f848_m1844._lb0344._0003.1','/hep300/data/khamano/data17/data17_13TeV.00331129.physics_Main.merge.AOD.f848_m1844/data17_13TeV.00331129.physics_Main.merge.AOD.f848_m1844._lb0395._0003.1','/hep300/data/khamano/data17/data17_13TeV.00331129.physics_Main.merge.AOD.f848_m1844/data17_13TeV.00331129.physics_Main.merge.AOD.f848_m1844._lb0638._0002.1']



############################
# Auto configure everything
############################
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections=InputFiles
ServiceMgr.PoolSvc.AttemptCatalogPatch = True;
ServiceMgr.OutputLevel=ERROR

#
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

if len(athenaCommonFlags.FilesInput()) == 0:
  athenaCommonFlags.FilesInput = ServiceMgr.EventSelector.InputCollections
    
#
import AthenaPoolCnvSvc.ReadAthenaPool
    
import AthenaPoolCnvSvc.WriteAthenaPool
    
#
from RecExConfig.RecFlags import rec

rec.AutoConfiguration          .set_Value_and_Lock(["everything"])

rec.abortOnUncheckedStatusCode .set_Value_and_Lock(False)
    
rec.doCBNT                     .set_Value_and_Lock(False)
rec.doESD                      .set_Value_and_Lock(False)
rec.doHist                     .set_Value_and_Lock(False)
rec.doPerfMon                  .set_Value_and_Lock(False)

rec.doWriteAOD                 .set_Value_and_Lock(False)
rec.doWriteESD                 .set_Value_and_Lock(False)
rec.doWriteTAG                 .set_Value_and_Lock(False)
rec.doWriteTAGCOM              .set_Value_and_Lock(False)

#
from AthenaCommon.Include import include
    
include ("RecExCommon/RecExCommon_topOptions.py")

#
for i in ServiceMgr:  i.OutputLevel=INFO

##############################
# Output file
##############################
OutputFile = 'Output_HLT_MetMon.root'

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["MonitorHLTmet DATAFILE='" + OutputFile + "' OPT='RECREATE'"]

##############################
# Add the Toplevel Alg
##############################

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

###############################
# Configure the job
###############################

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'

from RecExConfig.RecFlags import rec
rec.readRDO = False
rec.readESD = False
rec.readAOD = True
rec.doWriteESD = False
rec.doWriteAOD = False
rec.doWriteTAG = False

###############################
# Setup Athena Monitoring
###############################
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager

if not 'DQMonFlags' in dir():
  from AthenaMonitoring.DQMonFlags import DQMonFlags

HLTmetOutputLevel = INFO
myTrigDecisionTool = ""
if (DQMonFlags.useTrigger() and hasattr(ToolSvc, DQMonFlags.nameTrigDecTool())):
  myTrigDecisionTool = getattr(ToolSvc, DQMonFlags.nameTrigDecTool())
else:
  from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
  ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
  ToolSvc.TrigDecisionTool.OutputLevel=ERROR
  ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR

HLTmetMonMan = AthenaMonManager(name="HLTMonManager",
                                FileKey             = DQMonFlags.monManFileKey(),
                                Environment         = DQMonFlags.monManEnvironment(),
                                OutputLevel         = HLTmetOutputLevel)

topSequence += HLTmetMonMan

# in order to check available detectors
from RecExConfig.RecFlags  import rec

try:
  from TrigMETMonitoring.TrigMETMonitoringConfig import HLTMETMonitoringTool
  HLTmetMonMan.AthenaMonTools += HLTMETMonitoringTool()
except:
  print "Problems with HLTMETTool, tool not enabled"

HLTmetMonMan.FileKey = "MonitorHLTmet"

print HLTmetMonMan

#############################################
# Events to run over; "-1" for all events.
#############################################
theApp.EvtMax = -1

