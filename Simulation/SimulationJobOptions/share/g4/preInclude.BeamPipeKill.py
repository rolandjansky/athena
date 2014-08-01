#########################################################
#
# SimulationJobOptions/preInclude.BeamPipeKill.py
# Zach Marshall
#
# "Fast simulation" killing particles in forward beampipe
#
#########################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.BeamPipeCut = 0.
if hasattr(simFlags, 'BeamPipeSimMode' ):
    simFlags.BeamPipeSimMode = "FastSim"
