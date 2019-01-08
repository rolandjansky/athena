## Example configuration for FourLeptonMassFilter setting up defaults


if not hasattr( filtSeq, "FourLeptonInvMassFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import FourLeptonInvMassFilter
    filtSeq += FourLeptonInvMassFilter()
    pass

