# common fragment for xAODLepton filter
# conversion to XAOD, 
# creation of slimmed container containing electrons and muons
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")
createxAODSlimmedContainer("TruthElectrons",prefiltSeq)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

createxAODSlimmedContainer("TruthMuons",prefiltSeq)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

from GeneratorFilters.GeneratorFiltersConf import xAODLeptonFilter
xAODLeptonFilter = xAODLeptonFilter("xAODLeptonFilter")
filtSeq += xAODLeptonFilter

# to modify cuts put into JOs e.g.:
#filtSeq.xAODLeptonFilter.Ptcut = 12000.0
#filtSeq.xAODLeptonFilter.Etacut = 10.0
#filtSeq.xAODLeptonFilter.PtcutMax = 90000000000.0

