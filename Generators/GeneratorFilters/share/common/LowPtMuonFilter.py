## Instantiate the muon filter, including adding it to the stream requirement
## Configuration of the filter cuts is left to the specific JO
if not hasattr(filtSeq, "MuonFilter"):
    from GeneratorFilters.GeneratorFiltersConf import MuonFilter
    filtSeq += MuonFilter()

## Add this filter to the algs required to be successful for streaming
#if "MuonFilter" not in StreamEVGEN.RequireAlgs:
#    StreamEVGEN.RequireAlgs += ["MuonFilter"]

## Default cut params
filtSeq.MuonFilter.Ptcut = 3000.0
filtSeq.MuonFilter.Etacut = 2.8
