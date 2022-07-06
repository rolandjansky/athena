# common fragment for xAODDirectPhoton filter
# conversion to XAOD, 
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODDirectPhotonFilter
xAODDirectPhotonFilter = xAODDirectPhotonFilter("xAODDirectPhotonFilter")  
filtSeq += xAODDirectPhotonFilter

# to modiify cuts put into JOs sth. like:
#filtSeq.xAODDirectPhotonFilter.NPhotons = 1
#filtSeq.xAODDirectPhotonFilter.PtMin = 12000.0
#filtSeq.xAODDirectPhotonFilter.Ptmax = 35000.
#filtSeq.xAODDirectPhotonFilter.OrderPhotons = True
