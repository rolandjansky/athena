## Set some sim flags to take care of Neutron BG
from G4AtlasApps.SimFlags import simFlags
if 'runArgs' in dir() and hasattr(runArgs,'outputEvgenFile'):
    simFlags.StoppedParticleFile = runArgs.outputEvgenFile
else:
    simFlags.StoppedParticleFile = 'StoppedPartPositions.pool.root'

