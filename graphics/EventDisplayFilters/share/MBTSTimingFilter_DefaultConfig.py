#####################################################################################
# Add a filter algorithm that cuts on timing difference of the hits on MBTS A and C side
#####################################################################################
from EventDisplayFilters.EventDisplayFiltersConf import FilterUsingMBTSTiming
MBTSTimingFilter = FilterUsingMBTSTiming("FilterUsingMBTSTiming",OutputLevel=DEBUG)

# Default filter configuration
MBTSTimingFilter.OutputLevel = DEBUG
MBTSTimingFilter.MaxTimeDifference = 10 # Set maximum timing difference to 10ns
MBTSTimingFilter.CellChargeThreshold = 40./222. # set the cell charge threshold in pC

#Append in proper place, i.e. after Trigger-Bytestream decoding
index=0
for alg in topSequence:
  #count position
  index += 1
  #find index in the TrigBSExtraction in topSequence
  if (alg.getType() == 'CaloCellMaker'):
    topSequence.insert(index, MBTSTimingFilter)

#Make sure filtering is accepted
topSequence.StopOverride=False
