#########################################################
#
# SimulationJobOptions/preInclude.BeamPipe100.py
# Zach Marshall
#
# Range cuts for e+,e-,gamma in beamipe
#
#########################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.BeamPipeCut = 100.
if hasattr(simFlags, 'BeamPipeSimMode' ):
    simFlags.BeamPipeSimMode = "EGammaRangeCuts"
