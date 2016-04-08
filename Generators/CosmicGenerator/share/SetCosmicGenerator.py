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
