# Example JO to filter with DecaysFinalStateFilter

if not hasattr( filtSeq, "DecaysFinalStateFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import DecaysFinalStateFilter
    filtSeq += DecaysFinalStateFilter()
    pass

"""
DecaysFinalStateFilter = filtSeq.DecaysFinalStateFilter
DecaysFinalStateFilter.NQuarks = 2
DecaysFinalStateFilter.MinNChargedLeptons = 2
DecaysFinalStateFilter.PDGAllowedParents = [ 23, 24, -24 ]
"""
