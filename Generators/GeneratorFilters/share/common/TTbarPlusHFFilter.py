if not hasattr(filtSeq, "TTbarPlusBFilter"):
    from GeneratorFilters.GeneratorFiltersConf import TTbarPlusHeavyFlavorFilter
    filtSeq += TTbarPlusHeavyFlavorFilter("TTbarPlusBFilter")

## Default cut params
# filtSeq.TTbarPlusBFilter.SelectB = True  ## for tt+b
## topAlg.TTbarPlusBFilter.SelectC = True ## for tt+c
## topAlg.TTbarPlusBFilter.SelectL = True ## for tt+light

