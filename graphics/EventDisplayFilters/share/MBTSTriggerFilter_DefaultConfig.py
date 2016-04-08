#####################################################################################
# Add a filter algorithm that cuts on the multiplicity of MBTS hits
#####################################################################################
from EventDisplayFilters.EventDisplayFiltersConf import FilterUsingMBTS
MBTSFilter = FilterUsingMBTS("FilterUsingMBTS")

# Default filter configuration
MBTSFilter.OutputLevel = DEBUG
MBTSFilter.NumberOfAsideHits = 2 # Number of MBTSA triggers required for a pass
MBTSFilter.NumberOfCsideHits = 2 # Number of MBTSC triggers required for a pass
MBTSFilter.FirstMbtsCtpId    = 30 # ctpId of L1_MBTSA0  (30 is good for 20082009 beam)

#Append in proper place, i.e. after Trigger-Bytestream decoding
index=0
for alg in topSequence:
  #count position
  index += 1
  #find index in the TrigBSExtraction in topSequence
  if (alg.getType() == 'TrigBSExtraction'):
    topSequence.insert(index, MBTSFilter)

#Make sure filtering is accepted
topSequence.StopOverride=False
