#########################################################
#
# SimulationJobOptions/postInclude.Save7GeVpTPileUpTruthJets.py
# John Chapman
#
# Reconfigure MergeTruthJetsTool to save Truth Jets with pT > 7 GeV
# both in-time and out-of-time.
#
# This job option should be added via the postInclude
# command line argument. 
#
#########################################################
from AthenaCommon.CfgGetter import getPublicTool
mergetruthjetstool = getPublicTool("MergeTruthJetsTool")
from AthenaCommon.SystemOfUnits import GeV
mergetruthjetstool.InTimePtCut = 7.0 * GeV
mergetruthjetstool.OutOfTimePtCut = 7.0 * GeV
 
