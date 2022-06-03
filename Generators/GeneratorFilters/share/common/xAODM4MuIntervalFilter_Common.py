# common fragment for xAODM4MuInterval filter
# conversion to XAOD, 
# creation of slimmed container containing truth events
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODM4MuIntervalFilter
xAODM4MuIntervalFilter = xAODM4MuIntervalFilter("xAODM4MuIntervalFilter")  
filtSeq += xAODM4MuIntervalFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODM4MuIntervalFilter.MinPt = 12000.0
#filtSeq.xAODM4MuIntervalFilter.MaxEta = 5.0
#filtSeq.xAODM4MuIntervalFilter.LowM4muProbability = 1.0
#filtSeq.xAODM4MuIntervalFilter.MediumMj4muProbability = 0.3
#filtSeq.xAODM4MuIntervalFilter.HighM4muProbability = 0.1
#filtSeq.xAODM4MuIntervalFilter.LowM4mu = 11000.
#filtSeq.xAODM4MuIntervalFilter.HighM4mu = 25000.
#filtSeq.xAODM4MuIntervalFilter.ApplyReWeighting = True

