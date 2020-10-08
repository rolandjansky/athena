if not hasattr(filtSeq, "BoostedHadTopAndTopPair"):
    from GeneratorFilters.GeneratorFiltersConf import BoostedHadTopAndTopPair
    filtSeq += BoostedHadTopAndTopPair("BoostedHadTopAndTopPair")


## Choose to cut on the pT of the top on the list for m_cutPtOf = 0,
## or to cut on the pT of the top decay products (b, q, q'bar) on the list for m_cutPtOf = 1
filtSeq.BoostedHadTopAndTopPair.cutPtOf    = 1

## Default cut params (in MeV)
filtSeq.BoostedHadTopAndTopPair.tHadPtMin    =       0.0 # if -1 : all decay modes, if >= 0 : at least 1 hadronic top
filtSeq.BoostedHadTopAndTopPair.tHadPtMax    = 6000000.0
filtSeq.BoostedHadTopAndTopPair.tPairPtMin   =       0.0
filtSeq.BoostedHadTopAndTopPair.tPairPtMax   = 6000000.0
