#########################################################
##
## Filter cosmics events on muons leaving the calorimeter
##
## TODO: Move to the "cosmics" folder?
##
#########################################################

atlasG4log.info("Adding a filter on muons leaving the calorimeter")

from G4AtlasApps.SimFlags import simFlags
simFlags.CosmicFilterVolumeName = "Calo"
simFlags.CosmicFilterID = "13"
simFlags.CosmicFilterPTmin = "6000" # in MeV
simFlags.CosmicFilterPTmax = "50000" # in MeV
