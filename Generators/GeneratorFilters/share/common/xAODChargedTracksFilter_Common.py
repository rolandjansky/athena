# common fragment for xAODChargedTracks filter
# conversion to XAOD, 
# creation of slimmed container containing truth events
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODChargedTracksFilter
xAODChargedTracksFilter = xAODChargedTracksFilter("xAODChargedTracksFilter")  
filtSeq += xAODChargedTracksFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODChargedTracksFilter.Ptcut = 12000.0
#filtSeq.xAODChargedTracksFilter.EtaCut = 2.5
#filtSeq.xAODChargedTracksFilter.NTracks = 40


