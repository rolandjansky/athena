from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from AthenaCommon import CfgGetter
job.PileUpToolsAlg.PileUpTools += [  CfgGetter.getPrivateTool("ZDC_PileUpTool", checkType=True) ]
zdcPileUpTool = job.PileUpToolsAlg.PileUpTools[ "ZDC_PileUpTool" ]
