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

def cosmics_muonfilter():
    from G4AtlasApps import AtlasG4Eng
    include('G4CosmicFilter/G4CosmicFilter.py')

simFlags.InitFunctions.add_function("preInitG4", cosmics_muonfilter)
