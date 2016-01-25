#########################################################
#
# SimulationJobOptions/postOptions.ParticleID.py
# guennadi.pospelov@cern.ch
#
# Mods to have primary particle barcode signature on
# calorimeter calibration hits.
#########################################################

# TODO revisit this preInclude once ATLASSIM-2290 is complete
def particleid_setupcalos():
    from G4AtlasApps import AtlasG4Eng
    plar = AtlasG4Eng.G4Eng.Dict_DetConfig.get('PyLArG4RunControl')
    plar.LArG4GlobalOptions.SetDoCalibHitParticleID(True)
    plar.LArG4GlobalOptions.printMe()

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitDetFacility", particleid_setupcalos) #FIXME is this even valid now?
if hasattr(simFlags, "ParticleID"): simFlags.ParticleID=True
