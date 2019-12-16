## Example configuration for SUSYGGMHiggsinoPhotonFilter setting up defaults

if not hasattr( filtSeq, "SUSYGGMHiggsinoPhotonFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import SUSYGGMHiggsinoPhotonFilter
    filtSeq += SUSYGGMHiggsinoPhotonFilter()
    pass

"""
SUSYGGMHiggsinoPhotonFilter = filtSeq.SUSYGGMHiggsinoPhotonFilter

# Old method of pt cut - maintained for backwards compatibility
# SUSYGGMHiggsinoPhotonFilter.Ptcut = 10000.

SUSYGGMHiggsinoPhotonFilter.Ptmin = 10000.
SUSYGGMHiggsinoPhotonFilter.Ptmax = 100000000.
SUSYGGMHiggsinoPhotonFilter.Etacut = 2.50
SUSYGGMHiggsinoPhotonFilter.NPhotons = 1
SUSYGGMHiggsinoPhotonFilter.AllowSUSYDecay = False
"""
