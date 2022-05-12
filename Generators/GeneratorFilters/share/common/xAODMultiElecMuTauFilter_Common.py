# common fragment for xAODMultiLepton filter
# conversion to XAOD, 
# creation of slimmed container containing electrons and muons
# connecting the filter

if not hasattr(prefiltSeq, 'xAODCnv'):  
  from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
  prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
  prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'


from GeneratorFilters.GeneratorFiltersConf import xAODMultiElecMuTauFilter
xAODMultiElecMuTauFilter = xAODMultiElecMuTauFilter("xAODMultiElecMuTauFilter")
filtSeq += xAODMultiElecMuTauFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODMultiElecMuTauFilter.MinPt = 12000.0
#filtSeq.xAODMultiElecMuTauFilter.MaxEta = 10.0
#filtSeq.xAODMultiElecMuTauFilter.MinVisPtHadTau = 10000
#filtSeq.xAODMultiElecMuTauFilter.NLeptons = 4
#filtSeq.xAODMultiElecMuTauFilter.IncludeHadTaus = True
#filtSeq.xAODMultiElecMuTauFilter.TwoSameSignLightLeptonsOneHadTau = False 

