## Example configuration for MultiLeptonFilter setting up defaults

if not hasattr( filtSeq, "MultiLeptonFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import MultiLeptonFilter
    filtSeq += MultiLeptonFilter()
    pass

"""
MultiLeptonFilter = filtSeq.MultiLeptonFilter
MultiLeptonFilter.Ptcut = 10000.
MultiLeptonFilter.Etacut = 10.0
MultiLeptonFilter.NLeptons = 4
"""
