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
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

for alg in topSequence:
    if 'PileUpToolsAlg' in alg.name():
        alg.PileUpTools["MergeTruthJetsTool"].InTimePtCut = 7.0 * GeV
        alg.PileUpTools["MergeTruthJetsTool"].OutOfTimePtCut = 7.0 * GeV
        break
    if 'MergeTruthJets' == alg.name():
        alg.MergeTruthJetsTool.InTimePtCut = 7.0 * GeV
        alg.MergeTruthJetsTool.OutOfTimePtCut = 7.0 * GeV
        break
