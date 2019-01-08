if not hasattr(filtSeq, "TTbarPlusBBFilter"):
    from GeneratorFilters.GeneratorFiltersConf import TTbarPlusHeavyFlavorFilter
    filtSeq += TTbarPlusHeavyFlavorFilter("TTbarPlusBBFilter")
    filtSeq.TTbarPlusBBFilter.SelectB           = True
    filtSeq.TTbarPlusBBFilter.BMultiplicityCut  = 2 # >=
    filtSeq.TTbarPlusBBFilter.BpTMinCut         = 15000. # MeV
