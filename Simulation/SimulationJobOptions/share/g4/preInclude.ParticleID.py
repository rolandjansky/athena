#########################################################
#
# SimulationJobOptions/postOptions.ParticleID.py
# guennadi.pospelov@cern.ch
#
# Mods to have primary particle barcode signature on
# calorimeter calibration hits.
#########################################################

def particleid_setupcalos():
    from G4AtlasApps import AtlasG4Eng
    #atlasG4log.info("ParticleID Setting new LArG4GlobalOptions")
    plar = AtlasG4Eng.G4Eng.Dict_DetConfig.get('PyLArG4RunControl')
    plar.LArG4GlobalOptions.SetDoCalibHitParticleID(True)
    plar.LArG4GlobalOptions.printMe()
    #atlasG4log.info("ParticleID Setting new TileG4SimOptions")
    ptile = AtlasG4Eng.G4Eng.Dict_DetConfig.get('PyTileSimUtils')
    ptile.TileG4SimOptions.SetDoCalibHitParticleID(True)
    ptile.TileG4SimOptions.printMe()

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitDetFacility", particleid_setupcalos)

