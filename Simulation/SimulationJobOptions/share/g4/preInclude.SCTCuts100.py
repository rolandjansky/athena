#########################################################
#
# SimulationJobOptions/postInitOptions.SCTCuts100.py
# Pat Ward - adapted from postInitOptions.PixelCuts100.py by Zach Marshall
#
# For a special production to evaluate the effect of low
# energy secondaries on SCT Clustering.
#
# This job option should be added via the postInitInclude
# command line argument.
#
#########################################################

atlasG4log.info("G4 SCT Config: Setting SCT cut")

def sctcuts_setup():
    from G4AtlasApps import AtlasG4Eng
    ## Get both the main region and silicon region
    sct = AtlasG4Eng.G4Eng.Dict_DetFacility['SCT:SCT']
    sctr = sct.Dict_Regions['SCT']
    ## Stuff everything into the 'sensitive region'
    sctr.add_Volumes('SCT::SCT')
    ## Set cut for the new region
    sctr.Dict_Cuts = {'e-' : 0.100, 'gamma' : 0.100, 'e+' : 0.100}

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", sctcuts_setup)
