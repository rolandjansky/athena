#####################################################################################
# Add a filter algorithm that cuts on the number of SpacePoints
#####################################################################################
from EventDisplayFilters.EventDisplayFiltersConf import FilterUsingSpacePoints
SpacePointFilter = FilterUsingSpacePoints("FilterUsingSpacePoints")

# Default filter configuration
SpacePointFilter.OutputLevel = DEBUG
SpacePointFilter.NumberOfSpacePoints = 5 # Number of SpacePoints required

#Append in proper place, i.e. after Trigger-Bytestream decoding
index=0
for alg in topSequence:
  #count position
  index += 1
  #find index in the TrigBSExtraction in topSequence
  if (alg.name() == 'InDetSiTrackerSpacePointFinder'):
    topSequence.insert(index, SpacePointFilter)


#Make sure filtering is accepted
topSequence.StopOverride=False
