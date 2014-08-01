#########################################################
#
# SimulationJobOptions/preInclude.TRTCuts5.0.py
#
# For a special production to evaluate the effect of low
# energy deltas on TRT threshold tuning.
#
#########################################################

atlasG4log.info("G4 TRT Config: Setting TRT cut")

def trtcuts_setup():
    from G4AtlasApps import AtlasG4Eng
    ## Get both the main region and gas region
    trt = AtlasG4Eng.G4Eng.Dict_DetFacility['TRT:TRT']
    trtr = trt.Dict_Regions['TRT']
    ## Set cut for the old region
    trtr.Dict_Cuts['e-'] = 50.

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitDetFacility", trtcuts_setup)

