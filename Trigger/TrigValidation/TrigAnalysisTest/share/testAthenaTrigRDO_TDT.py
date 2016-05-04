from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

include("TriggerTest/testCommonSliceAthenaTrigRDO.py")

from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecision
trigDecWriter = WriteTrigDecision()


from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt
ToolSvc.TrigDecisionTool.OutputLevel=VERBOSE
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
topSequence.TDTExample.OutputLevel=DEBUG



