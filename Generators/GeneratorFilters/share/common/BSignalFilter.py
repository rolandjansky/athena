## Example configuration for MultiLeptonFilter setting up defaults

if not hasattr( filtSeq, "BSignalFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import BSignalFilter
    filtSeq += BSignalFilter()
    pass

"""
BSignalFilter.LVL1MuonCutOn = True
BSignalFilter.LVL2MuonCutOn = True
BSignalFilter.LVL1MuonCutPT = 3500
BSignalFilter.LVL1MuonCutEta = 2.6
BSignalFilter.LVL2MuonCutPT = 3500
BSignalFilter.LVL2MuonCutEta = 2.6
"""
