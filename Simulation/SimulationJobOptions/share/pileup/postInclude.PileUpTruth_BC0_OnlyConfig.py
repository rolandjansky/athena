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

from AthenaCommon.CfgGetter import getPublicTool
mergemceventcoll = getPublicTool("MergeMcEventCollTool")
mergemceventcoll.SaveOutOfTimeMinBias = False
mergemceventcoll.LowTimeToKeep = 0.0
mergemceventcoll.HighTimeToKeep = 25.0
