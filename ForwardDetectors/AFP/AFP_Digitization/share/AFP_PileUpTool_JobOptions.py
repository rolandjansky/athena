from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from AthenaCommon import CfgGetter
job.PileUpToolsAlg.PileUpTools += [  CfgGetter.getPrivateTool("AFP_PileUpTool", checkType=True) ]
afpPileUpTool = job.PileUpToolsAlg.PileUpTools[ "AFP_PileUpTool" ]
