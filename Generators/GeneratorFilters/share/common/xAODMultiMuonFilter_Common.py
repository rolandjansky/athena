# common fragment for xAODMultiMuon filter
# conversion to XAOD, 
# connecting the filter

if not hasattr(prefiltSeq, 'xAODCnv'):  
  from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
  prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
  prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

if not hasattr(prefiltSeq, "xAODTruthParticleSlimmerMuon"):
  from GeneratorFilters.GeneratorFiltersConf import xAODTruthParticleSlimmerMuon
  prefiltSeq += xAODTruthParticleSlimmerMuon('xAODTruthParticleSlimmerMuon')

from GeneratorFilters.GeneratorFiltersConf import xAODMultiMuonFilter
xAODMultiMuonFilter = xAODMultiMuonFilter("xAODMultiMuonFilter")  
filtSeq += xAODMultiMuonFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODMultiMuonFilter.Ptcut = 12000.0
#filtSeq.xAODMultiMuonFilter.Etacut = 10.0
#filtSeq.xAODMultiMuonFilter.NMuons = 2
