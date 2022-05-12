# common fragment for xAODElectron filter
# conversion to XAOD, 
# creation of slimmed container containing electrons
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")
createxAODSlimmedContainer("TruthElectrons",prefiltSeq)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

from GeneratorFilters.GeneratorFiltersConf import xAODElectronFilter
xAODElectronFilter = xAODElectronFilter("xAODElectronFilter")  
filtSeq += xAODElectronFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODElectronFilter.Ptcut = 12000.0
#filtSeq.xAODElectronFilter.Etacut = 10.0
