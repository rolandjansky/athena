## Example configuration for MassRangeFilter setting up defaults

if not hasattr( filtSeq, "MassRangeFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import MassRangeFilter
    filtSeq += MassRangeFilter()
    pass

"""
MassRangeFilter = filtSeq.MassRangeFilter
MassRangeFilter.PtCut = 10000.
MassRangeFilter.PtCut2 = 10000.
MassRangeFilter.EtaCut = 10.
MassRangeFilter.EtaCut2 = 10.
MassRangeFilter.InvMassMin = 0.
MassRangeFilter.InvMassMax = 14000000.
MassRangeFilter.PartId = 13
MassRangeFilter.PartId2 = 13
MassRangeFilter.PartStatus = 1
"""
