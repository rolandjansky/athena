## Example configuration for DiLeptonMassFilter setting up defaults

if not hasattr( filtSeq, "DiLeptonMassFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import DiLeptonMassFilter
    filtSeq += DiLeptonMassFilter()
    pass

"""
DiLeptonMassFilter = filtSeq.DiLeptonMassFilter
DiLeptonMassFilter.MinPt = 3500.
DiLeptonMassFilter.MaxEta = 3.0
DiLeptonMassFilter.MinMass = 4000.
DiLeptonMassFilter.MaxMass = 10000.
DiLeptonMassFilter.AllowSameCharge = False
"""
 
