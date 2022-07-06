# common fragment for xAODTTbarWithJpsimumu filter
# conversion to XAOD, 
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODTTbarWithJpsimumuFilter
xAODTTbarWithJpsimumuFilter = xAODTTbarWithJpsimumuFilter("xAODTTbarWithJpsimumuFilter")
filtSeq += xAODTTbarWithJpsimumuFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODTTbarWithJpsimumuFilter.SelectJpsi = True 
#filtSeq.xAODTTbarWithJpsimumuFilter.JpsipTMinCut = 5000. 
#filtSeq.xAODTTbarWithJpsimumuFilter.JpsietaMaxCut = 5. 



