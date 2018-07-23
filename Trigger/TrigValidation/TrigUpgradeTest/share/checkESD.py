include("AthenaPoolDataModelTest/esdtoesd_base.py")

svcMgr.EventSelector.InputCollections   = [ "myESD.pool.root" ]
from TrigValAlgs.TrigValAlgsConf import TrigEDMChecker
MessageSvc.defaultLimit = 9999999
MessageSvc.useColors = True
checker                                 = TrigEDMChecker()
checker.OutputLevel                     = DEBUG
checker.doDumpAll                       = False
checker.doDumpxAODTrigElectronContainer = False
checker.doDumpxAODTrackParticle         = True
checker.doDumpTrigCompsiteNavigation    = True
checker.doDumpStoreGate                 = True
# this list was obtained by: checkxAOD.py myESD.pool.root  | grep Composite | tr -s " "| cut -d" " -f10 | awk '{print "\""$1"\""}' | tr "\n" "," > f
# Note that now there is also a flag to look at all of them at runtime. We may want to move to this in the future.
tcContainers  =  [ "FilteredEMRoIDecisions", "L2CaloLinks", "L2ElectronLinks", "EgammaCaloDecisions", "FilteredEgammaCaloDecisions", "filterCaloRoIsAlg",  "ElectronL2Decisions", "MuonL2Decisions", "METRoIDecisions", "MURoIDecisions", "HLTChainsResult", "JRoIDecisions", "MonitoringSummaryStep1", "RerunEMRoIDecisions", "RerunMURoIDecisions", "TAURoIDecisions", "EMRoIDecisions"  ]

for container in tcContainers:
  checker.dumpTrigCompositeContainers += [ container + "_remap" ]

from AthenaCommon.AppMgr import topSequence
topSequence += checker
