if not hasattr(filtSeq, "MissingEtFilter"):
    from GeneratorFilters.GeneratorFiltersConf import MissingEtFilter
    filtSeq += MissingEtFilter("MissingEtFilter")
    
if not hasattr(filtSeq, "MissingEtFilterUpperCut"):
    filtSeq += MissingEtFilter("MissingEtFilterUpperCut")
    
filtSeq.MissingEtFilter.METCut = 60*GeV
filtSeq.MissingEtFilterUpperCut.METCut = 100000*GeV


filtSeq.Expression = "(MissingEtFilter and not MissingEtFilterUpperCut)"
