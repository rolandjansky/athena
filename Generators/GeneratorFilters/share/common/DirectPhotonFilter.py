## Example configuration for DirectPhotonFilter setting up defaults

if not hasattr( filtSeq, "DirectPhotonFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import DirectPhotonFilter
    filtSeq += DirectPhotonFilter()
    pass

"""
DirectPhotonFilter = filtSeq.DirectPhotonFilter

# Old method of pt cut - maintained for backwards compatibility
# DirectPhotonFilter.Ptcut = 10000.

DirectPhotonFilter.Ptmin = 10000.
DirectPhotonFilter.Ptmax = 100000000.
DirectPhotonFilter.Etacut = 2.50
DirectPhotonFilter.NPhotons = 1
DirectPhotonFilter.AllowSUSYDecay = False
"""
