# common fragment for xAODParentChildLepton filter
# conversion to XAOD, 
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODParentChildFilter
xAODParentChildFilter = xAODParentChildFilter("xAODParentChildFilter")
filtSeq += xAODParentChildFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODParentChildFilter.PtMinParent = 0.0
#filtSeq.xAODParentChildFilter.PtMaxParent = 1000000000.0
#filtSeq.xAODParentChildFilter.MassMinParent = 0.0
#filtSeq.xAODParentChildFilter.MassMaxParent = 0.0
#filtSeq.xAODParentChildFilter.PDGParent = [25]
#filtSeq.xAODParentChildFilter.EtaRangeParent = 10.0
#filtSeq.xAODParentChildFilter.PDGChild = [15]
#filtSeq.xAODParentChildFilter.PtMinChild = 0.0
#filtSeq.xAODParentChildFilter.EtaRangeChild = 0.0

