#########################################################
#
# SimulationJobOptions/preInclude.BeamPipeBP100.py
# Zach Marshall
#
# Adding 100 mm range cuts for e+,e-,gamma,p in beampipe
#
#########################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.BeamPipeCut = -100.
if hasattr(simFlags, 'BeamPipeSimMode' ):
    simFlags.BeamPipeSimMode = "EGammaPRangeCuts"
    simFlags.BeamPipeCut = 100.

