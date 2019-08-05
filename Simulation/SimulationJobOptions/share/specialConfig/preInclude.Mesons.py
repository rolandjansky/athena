#########################################################
#
# SimulationJobOptions/preInclude.Mesons.py
# Miha Muskinja
#
# Adds extra mesons to the Geant4 particle list. Mesons
# are configured in Mesons.MesonsList module.
#
#
#########################################################

## Add the physics tool for new mesons
simFlags.PhysicsOptions += ['MesonsPhysicsTool']

## Add the additional mesons to the white list
from G4AtlasApps.SimFlags import SimFlags
if hasattr(simFlags, 'ParticleSimWhiteList'):
    SimFlags.ParticleSimWhiteList.set_Value_and_Lock('ISF_ParticleSimWhiteList_LongLived')
