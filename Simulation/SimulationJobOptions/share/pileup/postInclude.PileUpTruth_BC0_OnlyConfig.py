#########################################################
#
# SimulationJobOptions/postOptions.PileUpTruth_BC0_OnlyConfig.py
#
# John Chapman
#
# Only keep truth information for BC0 (for non-truth-filtered
# background samples only).
#
# This job option should be added via the postInclude command line
# argument.
#
#########################################################

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

for alg in topSequence:
    if 'PileUpToolsAlg' in alg.name():
        alg.PileUpTools["MergeMcEventCollTool"].SaveOutOfTimeMinBias = False
        alg.PileUpTools["MergeMcEventCollTool"].LowTimeToKeep = 0.0
        alg.PileUpTools["MergeMcEventCollTool"].HighTimeToKeep = 25.0
        break
    if 'MergeMcEventCollection' == alg.name():
        alg.MergeMcEventCollTool.SaveOutOfTimeMinBias = False
        alg.MergeMcEventCollTool.LowTimeToKeep = 0.0
        alg.MergeMcEventCollTool.HighTimeToKeep = 25.0
        break
