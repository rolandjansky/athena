## Example configuration for TransverseMassVVFilter setting up defaults

if not hasattr( filtSeq, "TransverseMassVVFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import TransverseMassVVFilter
    filtSeq += TransverseMassVVFilter()
    pass

"""
TransverseMassVVFilter = filtSeq.TransverseMassVVFilter
TransverseMassVVFilter.MinMass = 4000.
TransverseMassVVFilter.MaxMass = 10000.
"""
 
