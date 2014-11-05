

from AthenaCommon.AlgSequence import AlgSequence
topSequence=AlgSequence()

#fix xAOD trigger object by scheduling cnv alg again

include( "RecExCond/RecExCommon_flags.py" )

from xAODTriggerCnv.xAODTriggerCnvConf import xAODMaker__TrigDecisionCnvAlg
topSequence += xAODMaker__TrigDecisionCnvAlg("TrigDecisionCnvAlg")
topSequence.TrigDecisionCnvAlg.xAODKey = 'xTrigDecision'

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )

