if not hasattr(filtSeq, "TTbarPlusBFilter"):
    from GeneratorFilters.GeneratorFiltersConf import TTbarPlusHeavyFlavorFilter
    filtSeq += TTbarPlusHeavyFlavorFilter("TTbarPlusBFilter")
    filtSeq.TTbarPlusBFilter.SelectB           = True
    filtSeq.TTbarPlusBFilter.BMultiplicityCut  = 1 # >=
    filtSeq.TTbarPlusBFilter.BpTMinCut         = 5000. # MeV
