if not hasattr(filtSeq, "TTbarPlusCFilter"):
    from GeneratorFilters.GeneratorFiltersConf import TTbarPlusHeavyFlavorFilter
    filtSeq += TTbarPlusHeavyFlavorFilter("TTbarPlusCFilter")
    filtSeq.TTbarPlusCFilter.SelectC           = True
    filtSeq.TTbarPlusCFilter.CMultiplicityCut  = 1 # >=
    filtSeq.TTbarPlusCFilter.CpTMinCut         = 15000. # MeV
