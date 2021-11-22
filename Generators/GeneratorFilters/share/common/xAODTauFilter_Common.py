# common fragment for xAODTau filter
# conversion to XAOD, 
# creation of slimmed container containing taus
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")
createxAODSlimmedContainer("TruthTaus",prefiltSeq)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

from GeneratorFilters.GeneratorFiltersConf import xAODTauFilter
#xAODTauFilter = xAODTauFilter("xAODTauFilter")  
#filtSeq += xAODTauFilter


