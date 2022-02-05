# common fragment for xAODElectron filter
# conversion to XAOD, 
# creation of slimmed container containing electrons
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")
createxAODSlimmedContainer("TruthElectrons",prefiltSeq)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

from GeneratorFilters.GeneratorFiltersConf import xAODElectronFilter
#xAODElectronFilter = xAODElectronFilter("xAODElectronFilter")  
#filtSeq += xAODElectronFilter


