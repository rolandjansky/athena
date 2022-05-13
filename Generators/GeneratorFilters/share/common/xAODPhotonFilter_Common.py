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
xAODPhotonFilter = xAODPhotonFilter("xAODPhotonFilter")  
filtSeq += xAODPhotonFilter

# to modiify cuts put into JOs sth. like:
#filtSeq.xAODPhotonFilter.PtMin = 12000.0
#filtSeq.xAODPhotonFilter.PtMax = 9000000000.0
#filtSeq.xAODPhotonFilter.EtaCut = 10.0
#filtSeq.xAODPhotonFilter.NPhotons = 2

