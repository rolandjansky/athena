## Set some sim flags to take care of Neutron BG
from G4AtlasApps.SimFlags import simFlags
if 'runArgs' in dir() and hasattr(runArgs,'outputEVNT_TRFile'):
    simFlags.StoppedParticleFile = runArgs.outputEVNT_TRFile
else:
    simFlags.StoppedParticleFile = 'StoppedPartPositions.pool.root'
jobproperties.Beam.beamType.set_Value_and_Lock('collisions')

# Modify stopping condition if desired
# -1 : ignore gasses
#simFlags.UserActionConfig.addConfig('G4UA::StoppedParticleActionTool','StoppingCondition',-1)
# -2 : ignore gasses, stop only negatively charged particles
#simFlags.UserActionConfig.addConfig('G4UA::StoppedParticleActionTool','StoppingCondition',-2)
