#########################################################
#
#  Turn off photon nuclear interactions in Geant4
#
#########################################################

atlasG4log.info("Turning off photon inelastic interactions")
def disable_photonuclear():
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.gbl.G4Commands().process.inactivate('PhotonInelastic', 'gamma')

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("postInit", disable_photonuclear)
