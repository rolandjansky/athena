# common fragment for xAODParentTwoChildren filter
# conversion to XAOD, 
# connecting the filter

if not hasattr(prefiltSeq, 'xAODCnv'):  
  from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
  prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
  prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

from GeneratorFilters.GeneratorFiltersConf import xAODParentTwoChildrenFilter
xAODParentTwoChildrenFilter = xAODParentTwoChildrenFilter("xAODParentTwoChildrenFilter")
filtSeq += xAODParentTwoChildrenFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODParentTwoChildrenFilter.PtMinParent = 0.0
#filtSeq.xAODParentTwoChildrenFilter.PDGParent = [25]
#filtSeq.xAODParentTwoChildrenFilter.EtaRangeParent = 10.0
#filtSeq.xAODParentTwoChildrenFilter.PDGChild = [5,22]
#filtSeq.xAODParentTwoChildrenFilter.PtMinChild = 0.0
#filtSeq.xAODParentTwoChildrenFilter.EtaRangeChild = 0.0

