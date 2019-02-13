from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
if not hasattr(prefiltSeq, 'xAODCnv'):
    prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

# Turn off ghost association algorithms
include("GeneratorFilters/JetFilter_JZX.py")
#from JetFilter_JZX import JZSlice

JZSlice(0,prefiltSeq,filtSeq,runArgs.ecmEnergy,0.6)

