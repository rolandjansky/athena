################################################
##
## SimulationJobOptions/preInclude.HaloSim.py
##
## Original Author: Borut Kersevan
## Adapted for MC11 and MC12: John Chapman
## Fragment to increase world volume dimensions. 
## Needed by beam halo simulation. 
################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.WorldZRange.set_Value(26050)
simFlags.WorldRRange.set_Value(16000)
## No vertex position smearing or filtering on pseudorapidity.  Only
## check if G4PrimaryVertex objects are within the ATLAS volume.
simFlags.EventFilter.switchAllFiltersOff()
simFlags.EventFilter.switchFilterOn('VertexRangeChecker')
simFlags.VertexFromCondDB.set_Off()
simFlags.VertexTimeOffset.set_Off()

# Hit filter setup function for callback
def hitFilterSetup():
  atlasG4log.info("Setting up filter parameters")
  include("G4HitFilter/G4HitFilter.py")

# Register the callback
simFlags.InitFunctions.add_function("preInitG4", hitFilterSetup)
