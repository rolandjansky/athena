from glob import glob

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = glob(inputFiles)


#ToolSvc += CfgMgr.TrigConf__xAODConfigTool("xAODConfigTool")
#ToolSvc += CfgMgr.Trig__TrigDecisionTool("TrigDecisionTool",
                                         #ConfigTool = ToolSvc.xAODConfigTool,
                                         #TrigDecisionKey = "xTrigDecision",
                                         #OutputLevel = 1)

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
trigDecTool = Trig__TrigDecisionTool('TrigDecisionTool')
ToolSvc += trigDecTool

svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=1000)

from AthenaCommon.AlgSequence import AlgSequence
algSeq = AlgSequence()

from TrigTauMatching.TrigTauMatchingConf import Trig__TrigTauMatchingTool
tauMatchingTool = Trig__TrigTauMatchingTool('TrigTauMatchingTool');
tauMatchingTool.TrigDecisionTool = trigDecTool
ToolSvc += tauMatchingTool

from TrigTauMatching.TrigTauMatchingConf import TrigTauMatching_example
alg = TrigTauMatching_example()
alg.TrigTauMatchingTool = tauMatchingTool

algSeq += alg
