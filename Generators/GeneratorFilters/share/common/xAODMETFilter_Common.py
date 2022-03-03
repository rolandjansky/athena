# common fragment for xAODMissingEt filter
# conversion to XAOD, 
# creation of slimmed container containing MET
# connecting the filter

if not hasattr(prefiltSeq, 'xAODCnv'):  
  from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
  prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
  prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

if not hasattr(prefiltSeq, "xAODTruthParticleSlimmerMET"):
  from GeneratorFilters.GeneratorFiltersConf import xAODTruthParticleSlimmerMET
  prefiltSeq += xAODTruthParticleSlimmerMET('xAODTruthParticleSlimmerMET')

from GeneratorFilters.GeneratorFiltersConf import xAODMETFilter
#xAODMissingEtFilter = xAODMissingEtFilter("xAODMissingEtFilter")  
#filtSeq += xAODMissingEtFilter


