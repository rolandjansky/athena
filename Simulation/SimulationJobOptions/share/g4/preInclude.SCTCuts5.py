#########################################################
#
# SimulationJobOptions/postInitOptions.SCTCuts5.py
# Pat Ward - adapted from postInitOptions.PixelCuts5.py by Zach Marshall
#
# For a special production to evaluate the effect of low
# energy secondaries on SCT Clustering.
#
#########################################################

from G4AtlasApps import PyG4Atlas, AtlasG4Eng
atlasG4log.info("G4 SCT Config: Setting SCT cut")

def sctcuts_setup():
    ## Get both the main region and silicon region
    sct = AtlasG4Eng.G4Eng.Dict_DetFacility['SCT:SCT']
    sctr = sct.Dict_Regions['SCT']
    ## Stuff everything into the 'sensitive region'
    sctr.add_Volumes('SCT::SCT')
    ## Set cut for the new region
    sctr.Dict_Cuts = {'e-' : 0.005, 'gamma' : 0.005, 'e+' : 0.005}

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", sctcuts_setup)
