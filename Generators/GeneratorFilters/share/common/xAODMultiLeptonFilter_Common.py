# common fragment for xAODMultiLepton filter
# conversion to XAOD, 
# creation of slimmed container containing electrons and muons
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")
createxAODSlimmedContainer("TruthElectrons",prefiltSeq)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

createxAODSlimmedContainer("TruthMuons",prefiltSeq)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

from GeneratorFilters.GeneratorFiltersConf import xAODMultiLeptonFilter
xAODMultiLeptonFilter = xAODMultiLeptonFilter("xAODMultiLeptonFilter")
filtSeq += xAODMultiLeptonFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODMultiLeptonFilter.Ptcut = 12000.0
#filtSeq.xAODMultiLeptonFilter.Etacut = 10.0
#filtSeq.xAODMultiLeptonFilter.NLeptons = 4

