#########################################################
#
# SimulationJobOptions/preInclude.ExtraParticles.py
# author: Miha Muskinja
# date:   August 2019
#
# adds extra particles to the Geant4 particle list
# particles are read from the PDGTABLE
#
#
#########################################################

## Add the physics tool
simFlags.PhysicsOptions += ['ExtraParticlesPhysicsTool']

## Add the additional ExtraParticles to the white list
from G4AtlasApps.SimFlags import SimFlags
if hasattr(simFlags, 'ParticleSimWhiteList'):
    SimFlags.ParticleSimWhiteList.set_Value_and_Lock('ISF_ParticleSimWhiteList_ExtraParticles')
