PoolAODInput=["/afs/cern.ch/user/t/tbold/public/TDTtest/attila.AOD.pool.root"]
# PoolAODInput=['/space2/tkohno/data/test/valid1.105144.PythiaZee.recon.AOD.e380_s513_r634_tid043141/AOD.043141._00005.pool.root.1']

import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections=PoolAODInput
ServiceMgr.OutputLevel=ERROR
for i in ServiceMgr:  i.OutputLevel=ERROR
#
# TrigDecisionTool
#
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
tdt.OutputLevel=ERROR
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR
tdt.PublicChainGroups = {"EFTau": "EF_[0-9]?tau.*",
                         "EFPhoton": "EF_[0-9]?g*",
                         "EFJets":"EF_J.*",
                         }


#
# Example Code
#
from TrigAnalysisExamples.TrigAnalysisExamplesConf import Trig__TDTExample
from TrigAnalysisExamples.TDTExample import TriggerTree
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += Trig__TDTExample("TDTExample")
#? is this necessary if we use the public tdt?
####topSequence.TDTExample.TrigDecisionTool = tdt
topSequence.TDTExample.OutputLevel=ERROR
# topSequence += TriggerTree('trigger')

###############################

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()

from RecExConfig.RecFlags import rec
rec.readRDO=False
rec.readAOD=True
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doWriteTAG=False

#
# TrigConfigSvc
#
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList=['ds']
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter()

from TriggerMenuAnalysis.TriggerMenuAnalysisConfig import TestNewTDT_default
theTestNewTDT = TestNewTDT_default()
theTestNewTDT.OutputLevel = INFO
topSequence += theTestNewTDT

#
# Add Histograms
#
ServiceMgr += CfgMgr.THistSvc()
ServiceMgr.THistSvc.Output += ["turnontree DATAFILE='TDTExample.root' TYP='ROOT' OPT='RECREATE'"]
ServiceMgr.THistSvc.OutputLevel = ERROR

theApp.EvtMax=-1
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
theApp.OutputLevel=ERROR

print 'Printing ServiceMgr'
print ServiceMgr
print 'Printing topSequence'
print topSequence
