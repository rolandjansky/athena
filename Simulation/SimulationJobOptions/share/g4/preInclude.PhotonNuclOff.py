#########################################################
#
#  Turn off photon nuclear interactions in Geant4
#
#########################################################

atlasG4log.info("Turning off photon inelastic interactions")
from G4AtlasApps.SimFlags import simFlags
simFlags.G4Commands += ['/process/inactivate PhotonInelastic gamma']
