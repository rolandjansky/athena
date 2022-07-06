# common fragment for xAODMultiMuon filter
# conversion to XAOD, 
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")
createxAODSlimmedContainer("TruthMuons",prefiltSeq)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

from GeneratorFilters.GeneratorFiltersConf import xAODMultiMuonFilter
xAODMultiMuonFilter = xAODMultiMuonFilter("xAODMultiMuonFilter")  
filtSeq += xAODMultiMuonFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODMultiMuonFilter.Ptcut = 12000.0
#filtSeq.xAODMultiMuonFilter.Etacut = 10.0
#filtSeq.xAODMultiMuonFilter.NMuons = 2
