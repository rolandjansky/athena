## Example configuration for MultiMuonFilter setting up defaults

if not hasattr( filtSeq, "MultiMuonFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import MultiMuonFilter
    filtSeq += MultiMuonFilter()
    pass

"""
MultiMuonFilter = filtSeq.MultiMuonFilter
MultiMuonFilter.Ptcut = 4000.
MultiMuonFilter.Etacut = 2.7
MultiMuonFilter.NMuons = 4
"""
 
