# common fragment for xAODMuon filter
# conversion to XAOD, 
# creation of slimmed container containing muons
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")
createxAODSlimmedContainer("TruthMuons",prefiltSeq)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

from GeneratorFilters.GeneratorFiltersConf import xAODMuonFilter
xAODMuonFilter = xAODMuonFilter("xAODMuonFilter")
filtSeq += xAODMuonFilter

# to modiify cuts put into JOs e.g.:  
#filtSeq.xAODMuonFilter.Ptcut = 12000.0 
#filtSeq.xAODMuonFilter.Etacut = 10.0

