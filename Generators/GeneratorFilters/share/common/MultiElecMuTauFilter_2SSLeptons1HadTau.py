##
## Example configuration for MultiElecMuTauFilter setting up the selection of defaults: 2 Same Sign Light Leptons  + 1 Hadronic Tau. 
## NLeptons = 3
##

if not hasattr(filtSeq, "MultiElecMuTauFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
    filtSeq += MultiElecMuTauFilter()
    pass

# Setup defaults as example
"""
MultiElecMuTauFilter = filtSeq.MultiElecMuTauFilter
MultiElecMuTauFilter.NLeptons  = 3
MultiElecMuTauFilter.MaxEta = 10.0
MultiElecMuTauFilter.MinPt = 10000.0
MultiElecMuTauFilter.MinVisPtHadTau = 20000.0
MultiElecMuTauFilter.IncludeHadTaus = True
MultiElecMuTauFilter.TwoSameSignLightLeptonsOneHadTau = True
"""
