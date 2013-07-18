from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from AthenaCommon import CfgGetter
job.PileUpToolsAlg.PileUpTools += [  CfgGetter.getPrivateTool("LUCID_PileUpTool", checkType=True) ]
lucidPileUpTool = job.PileUpToolsAlg.PileUpTools[ "LUCID_PileUpTool" ]
