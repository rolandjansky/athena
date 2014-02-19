from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from AthenaCommon import CfgGetter
job.PileUpToolsAlg.PileUpTools += [  CfgGetter.getPrivateTool("ALFA_PileUpTool", checkType=True) ]
alfaPileUpTool = job.PileUpToolsAlg.PileUpTools[ "ALFA_PileUpTool" ]
