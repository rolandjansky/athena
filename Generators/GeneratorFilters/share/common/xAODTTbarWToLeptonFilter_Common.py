# common fragment for xAODTTbarWToLepton filter
# conversion to XAOD, 
# connecting the filter

if not hasattr(prefiltSeq, 'xAODCnv'):  
  from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
  prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
  prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

from GeneratorFilters.GeneratorFiltersConf import xAODTTbarWToLeptonFilter
xAODTTbarWToLeptonFilter = xAODTTbarWToLeptonFilter("xAODTTbarWToLeptonFilter")
filtSeq += xAODTTbarWToLeptonFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODTTbarWToLeptonFilter.Ptcut = 200000 
#filtSeq.xAODTTbarWToLeptonFilter.NumLeptons = -1 # Negative for >0, positive integers for the specific number
#filtSeq.xAODTTbarWToLeptonFilter.fourTopsFilter = False
#filtSeq.xAODTTbarWToLeptonFilter.SSMLFilter = False



