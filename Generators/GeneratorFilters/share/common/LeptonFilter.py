## Instantiate the lepton filter, including adding it to the stream 
## Configuration of the filter cuts is left to the specific JO
if not hasattr(filtSeq, "LeptonFilter"):
    from GeneratorFilters.GeneratorFiltersConf import LeptonFilter
    filtSeq += LeptonFilter()

## Add this filter to the algs required to be successful for streaming
#if "LeptonFilter" not in StreamEVGEN.RequireAlgs:
#    StreamEVGEN.RequireAlgs += ["LeptonFilter"]

## Default cut params
filtSeq.LeptonFilter.Ptcut = 10000.
filtSeq.LeptonFilter.Etacut = 2.8

