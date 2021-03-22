if not hasattr(prefiltSeq, 'xAODCnv'):
  from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
  prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
  prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

if not hasattr(prefiltSeq, "PUTruthParticleSlimmer"):
  from GeneratorFilters.GeneratorFiltersConf import PileupTruthParticleSlimmer
  prefiltSeq += PileupTruthParticleSlimmer('PUTruthParticleSlimmer')

