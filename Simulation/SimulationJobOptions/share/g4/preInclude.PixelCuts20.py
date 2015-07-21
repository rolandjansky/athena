#########################################################
#
# SimulationJobOptions/postInitOptions.PixelCuts20.py
# Zach Marshall
#
# For a special production to evaluate the effect of low
# energy secondaries on Pixel Clustering.
#
#########################################################

atlasG4log.info("G4 PIX Config: Setting PIX cut")

def pixcuts_setup():
    from G4AtlasApps import AtlasG4Eng
    ## Get both the main region and silicon region
    pix = AtlasG4Eng.G4Eng.Dict_DetFacility['PIX:PIX']
    pixr = pix.Dict_Regions['PIX']
    ## Stuff everything into the 'sensitive region'
    pixr.add_Volumes('PIX::PIX')
    ## Set cut for the new region
    pixr.Dict_Cuts = {'e-' : 0.020, 'gamma' : 0.020, 'e+' : 0.020}

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", pixcuts_setup)
