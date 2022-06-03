# common fragment for xAODForwardProton filter
# conversion to XAOD, 
# creation of slimmed container containing truth events
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODForwardProtonFilter
xAODForwardProtonFilter = xAODForwardProtonFilter("xAODForwardProtonFilter")  
filtSeq += xAODForwardProtonTimeFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODForwardProtonFilter.xi_min = 0.00
#filtSeq.xAODForwardProtonFilter.xi_max = 0.20
#filtSeq.xAODForwardProtonFilter.beam_energy = 6500.*GeV
#filtSeq.xAODForwardProtonFilter.pt_min = 0.5*GeV
#filtSeq.xAODForwardProtonFilter.pt_max = 1.5*GeV


