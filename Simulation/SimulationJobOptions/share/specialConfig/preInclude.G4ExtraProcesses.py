#########################################################
#
# SimulationJobOptions/preInclude.G4ExtraProcesses.py
# author: Miha Muskinja
# date:   August 2020
#
# adds extra processes to particles already present
# in Geant4.
#
#
#########################################################

## Add the physics tool
simFlags.PhysicsOptions += ['G4EMProcessesPhysicsTool']
