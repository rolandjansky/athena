######################################################################
## SimulationJobOptions/preInclude.VerboseBeamEffectTransformation.py
##
## Use verbose output for the BeamEffectTransformation. (Can be easily adapted
## for use with the other EventFilters if required.)
######################################################################

def verbose_beameffecttransformation():
    print "verbose BeamEffectTransformation"
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.menu_EventFilter().getFilter('BeamEffectTransformation').SetVerboseLevel(99)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", verbose_beameffecttransformation)
