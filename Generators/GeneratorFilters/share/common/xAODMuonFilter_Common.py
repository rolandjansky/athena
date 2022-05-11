# common fragment for xAODMuon filter
# conversion to XAOD, 
# creation of slimmed container containing muons
# connecting the filter

if not hasattr(prefiltSeq, 'xAODCnv'):  
  from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
  prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
  prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

if not hasattr(prefiltSeq, "xAODTruthParticleSlimmerMuon"):
  from GeneratorFilters.GeneratorFiltersConf import xAODTruthParticleSlimmerMuon
  prefiltSeq += xAODTruthParticleSlimmerMuon('xAODTruthParticleSlimmerMuon')

from GeneratorFilters.GeneratorFiltersConf import xAODMuonFilter
xAODMuonFilter = xAODMuonFilter("xAODMuonFilter")
filtSeq += xAODMuonFilter

# to modiify cuts put into JOs e.g.:  
#filtSeq.xAODMuonFilter.Ptcut = 12000.0 
#filtSeq.xAODMuonFilter.Etacut = 10.0

