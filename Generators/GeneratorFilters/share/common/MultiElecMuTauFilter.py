##
## Example configuration for MultiElecMuTauFilter setting up defaults
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
MultiElecMuTauFilter.MinPt = 5000.0
MultiElecMuTauFilter.MinVisPtHadTau = 10000.0
MultiElecMuTauFilter.IncludeHadTaus = True
"""
