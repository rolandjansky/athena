"""
Set up cosmic generator for simulation + trigger.
"""

## Configuring the Athena application for a 'generator' job
from G4AtlasApps.SimFlags import simFlags
simFlags.load_cosmics_flags()
assert hasattr(simFlags, "ReadTR")

## Set up standard algorithms and random seeds
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.CfgGetter import getAlgorithm
if simFlags.ReadTR.statusOn:
    try:
        cosmicGen = topSequence.TrackRecordCosmicGenerator
    except:
        cosmicGen = getAlgorithm("TrackRecordCosmicGenerator")
else:
    try:
        cosmicGen = topSequence.EvgenCosmicGenerator
    except:
        cosmicGen = getAlgorithm("EvgenCosmicGenerator")

## Adding the McEventCollection dumper to the list of algs
#from TruthExamples.TruthExamplesConf import DumpMC
#topSequence += DumpMC()

print cosmicGen

#FIXME temporary hacks to ensure that BeamEffectsAlg runs after the Cosmic Generators
def moveAlgInSequence(alg, seq, newpos):
    l = seq.getChildren()
    delattr(seq, alg.getName())
    seq.insert(newpos, alg)

def checkCosmicAlgPosition(beamAlg, cosmicAlg):
    topSeq = AlgSequence()

    if cosmicAlg in topSeq:
        cosmicInd = topSeq.getChildren().index(cosmicAlg)
        if beamAlg in topSeq:
            index = topSeq.getChildren().index(beamAlg)
            # make sure cosmicAlg is before index
            if cosmicInd > index:
                moveAlgInSequence(cosmicAlg, topSeq, index)
                cosmicInd = index
        #ensure that timinAlg is before cosmicAlg
        timingAlg=None
        if hasattr(topSeq,"SimTimerBegin"):
            timingAlg = topSeq.SimTimerBegin
        if timingAlg is not None and timingAlg in topSeq:
            index = topSeq.getChildren().index(timingAlg)
            if index > cosmicInd:
                moveAlgInSequence(timinAlg, topSeq, cosmicInd)
if hasattr(topSequence, 'BeamEffectsAlg'):
    checkCosmicAlgPosition(topSequence.BeamEffectsAlg, cosmicGen)
