# common fragment for xAODPhoton filter
# conversion to XAOD, 
# creation of slimmed container containing photons
# connecting the filter

if not hasattr(prefiltSeq, 'xAODCnv'):  
  from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
  prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
  prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

if not hasattr(prefiltSeq, "xAODTruthParticleSlimmerPhoton"):
  from GeneratorFilters.GeneratorFiltersConf import xAODTruthParticleSlimmerPhoton
  prefiltSeq += xAODTruthParticleSlimmerPhoton('xAODTruthParticleSlimmerPhoton')

from GeneratorFilters.GeneratorFiltersConf import xAODPhotonFilter
#xAODPhotonFilter = xAODPhotonFilter("xAODPhotonFilter")  
#filtSeq += xAODPhotonFilter


