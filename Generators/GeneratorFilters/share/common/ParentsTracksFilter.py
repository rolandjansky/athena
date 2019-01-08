if not hasattr(filtSeq, "ParentsTracksFilter"):
    from GeneratorFilters.GeneratorFiltersConf import ParentsTracksFilter
    filtSeq += ParentsTracksFilter("ParentsTracksFilter")

## Add this filter to the algs required to be successful for streaming
#if "ParentsTracksFilter" not in StreamEVGEN.RequireAlgs:
#    StreamEVGEN.RequireAlgs += ["ParentsTracksFilter"]

## Default cut params
filtSeq.ParentsTracksFilter.PDGParent  = [511,521,531,541,5122,5132,5232,5332]
filtSeq.ParentsTracksFilter.PtMinParent =  0.
filtSeq.ParentsTracksFilter.PtMaxParent =  1e9
filtSeq.ParentsTracksFilter.MassMinParent = -1e9
filtSeq.ParentsTracksFilter.MassMaxParent =  1e9
filtSeq.ParentsTracksFilter.EtaRangeParent = 10.0
filtSeq.ParentsTracksFilter.PtMinLeptons = 0.
filtSeq.ParentsTracksFilter.EtaRangeLeptons = 1000.
filtSeq.ParentsTracksFilter.PtMinHadrons = 0.
filtSeq.ParentsTracksFilter.EtaRangeHadrons = 1000.
filtSeq.ParentsTracksFilter.NumMinTracks = 0
filtSeq.ParentsTracksFilter.NumMaxTracks = 999999
filtSeq.ParentsTracksFilter.NumMinLeptons = 0
filtSeq.ParentsTracksFilter.NumMaxLeptons = 999999
filtSeq.ParentsTracksFilter.NumMinOthers = 0
filtSeq.ParentsTracksFilter.NumMaxOthers = 999999
