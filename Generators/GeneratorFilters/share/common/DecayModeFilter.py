if not hasattr(filtSeq, "DecayModeFilter"):
   from GeneratorFilters.GeneratorFiltersConf import DecayModeFilter
   filtSeq += DecayModeFilter()
   pass

filtSeq.DecayModeFilter.modeID = "E"
