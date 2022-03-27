# common fragment for xAODDiLeptonMass filter
# conversion to XAOD, 
# creation of slimmed container containing electrons
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")
createxAODSlimmedContainer("TruthLightLeptons",prefiltSeq)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

from GeneratorFilters.GeneratorFiltersConf import xAODDiLeptonMassFilter


