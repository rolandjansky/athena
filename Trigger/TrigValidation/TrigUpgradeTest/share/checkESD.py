include("AthenaPoolDataModelTest/esdtoesd_base.py")

svcMgr.EventSelector.InputCollections   = [ "myESD.pool.root" ]
from TrigValAlgs.TrigValAlgsConf import TrigEDMChecker
checker                                 = TrigEDMChecker()
checker.OutputLevel                     = DEBUG
checker.doDumpAll                       = False
checker.doDumpxAODTrigElectronContainer = True
checker.doDumpxAODTrackParticle         = True
# this list was obtained by: checkxAOD.py myESD.pool.root  | grep Composite | tr -s " "| cut -d" " -f10 | awk '{print "\""$1"\""}' | tr "\n" "," > f
checker.dumpTrigCompositeContainers     = ["MURoIDecisions","MuonL2Decisions","ElectronL2Decisions"
                                           ,"HLTSummary","EMRoIDecisions","METRoIDecisions","HLTChainsResult",
                                           "EgammaCaloDecisions","MURoIDecisions","MuonL2Decisions",
                                           "ElectronL2Decisions","HLTSummary","METRoIDecisions",
                                           "EgammaCaloDecisions","EMRoIDecisions","HLTChainsResult"]

from AthenaCommon.AppMgr import topSequence
topSequence += checker
