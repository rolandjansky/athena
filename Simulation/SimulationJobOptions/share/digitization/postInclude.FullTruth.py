##
## postInclude.FullTruth.py
##

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

for alg in topSequence:
    if 'PileUpToolsAlg' in alg.name():
        alg.PileUpTools["MergeMcEventCollTool"].DoSlimming = False
        break
    if 'MergeMcEventCollection' == alg.name():
        alg.MergeMcEventCollTool.DoSlimming = False
        break

##EOF
