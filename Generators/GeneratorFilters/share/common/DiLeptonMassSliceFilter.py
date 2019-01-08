## Example configuration for DiLeptonMassSliceFilter setting up defaults

if not hasattr( filtSeq, "DiLeptonMassSliceFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import DiLeptonMassSliceFilter
    filtSeq += DiLeptonMassSliceFilter()
    pass

"""
DiLeptonMassSliceFilter = filtSeq.DiLeptonMassSliceFilter
DiLeptonMassSliceFilter.MinPt = 3500.
DiLeptonMassSliceFilter.MaxEta = 3.0
DiLeptonMassSliceFilter.MinMass = 4000.
DiLeptonMassSliceFilter.LowerThenMassCut = 86188.
DiLeptonMassSliceFilter.GreaterThenMassCut = 96188.
DiLeptonMassSliceFilter.AllowSameCharge = False
"""
 
