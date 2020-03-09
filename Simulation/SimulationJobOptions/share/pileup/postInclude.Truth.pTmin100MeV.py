#########################################################
#
# SimulationJobOptions/postOptions.Truth.pTmin100MeV.py
# John Chapman
#
# For very low luminosities keep truth information for
# particles with pT>100MeV rather than pT>400MeV.
#
# This job option should be added via the postInclude
# command line argument.
#
#########################################################

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

for alg in topSequence:
    if 'PileUpToolsAlg' in alg.name():
        alg.PileUpTools["MergeMcEventCollTool"].ptMin = 100.0 #400.0 MeV
        break
    if 'MergeMcEventCollection' == alg.name():
        alg.MergeMcEventCollTool.ptMin = 100.0 #400.0 MeV
        break
