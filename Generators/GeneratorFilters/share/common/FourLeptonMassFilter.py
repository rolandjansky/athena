## Example configuration for FourLeptonMassFilter setting up defaults


if not hasattr( filtSeq, "FourLeptonMassFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import FourLeptonMassFilter
    filtSeq += FourLeptonMassFilter()
    pass

## Setup Defaults as example
"""
FourLeptonMassFilter = filtSeq.FourLeptonMassFilter
FourLeptonMassFilter.MinPt = 5000.
FourLeptonMassFilter.MaxEta = 3.
FourLeptonMassFilter.MinMass1 = 60000.
FourLeptonMassFilter.MaxMass1 = 14000000.
FourLeptonMassFilter.MinMass2 = 12000.
FourLeptonMassFilter.MaxMass2 = 14000000.
FourLeptonMassFilter.AllowElecMu = True
FourLeptonMassFilter.AllowSameCharge = True
"""
