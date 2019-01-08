## Example configuration setting up JetFilter defaults 

if not hasattr( filtSeq, "JetFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import JetFilter
    filtSeq += JetFilter()
    pass

filtSeq.JetFilter.JetNumber = 1
filtSeq.JetFilter.EtaRange = 2.7
filtSeq.JetFilter.JetThreshold = 17000.
filtSeq.JetFilter.JetType = False # True = cone, False = grid
filtSeq.JetFilter.GridSizeEta = 2 # Number of (approx 0.06 size) eta cells
filtSeq.JetFilter.GridSizePhi = 2 # Number of (approx 0.06 size) phi cells
