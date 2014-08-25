
include.block( "MissingETPerformance/PileUp_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import PileUpTool
pileUpTool = PileUpTool()

# Modify MissingETData to get true EtMiss with pile-up
#ToolSvc.MissingETData.METTruthKey = 'MET_Truth_PileUp'

ToolSvc += pileUpTool
print      pileUpTool
