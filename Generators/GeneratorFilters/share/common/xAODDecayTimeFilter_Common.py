# common fragment for xAODDecayTime filter
# conversion to XAOD, 
# creation of slimmed container containing truth events
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODDecayTimeFilter
xAODDecayTimeFilter = xAODDecayTimeFilter("xAODDecayTimeFilter")  
filtSeq += xAODDecayTimeFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODDecayTimeFilter.PDGs = [ 511, 521 ]
#filtSeq.xAODDecayTimeFilter.LifetimeLow = 8.0
#filtSeq.xAODDecayTimeFilter.LifetimeHigh = 999999.




