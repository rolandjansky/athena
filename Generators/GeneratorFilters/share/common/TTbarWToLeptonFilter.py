## Instantiate the TTbarWToLeptonFilter, including adding it to the stream requirement
## Filter passed if top, antitop and t->W->e/mu/tau in truth chain
## Configuration of the filter cuts is left to the specific JO
## christoph.wasicki@cern.ch

if not hasattr(filtSeq, "TTbarWToLeptonFilter"):
    from GeneratorFilters.GeneratorFiltersConf import TTbarWToLeptonFilter
    filtSeq += TTbarWToLeptonFilter()

## Add this filter to the algs required to be successful for streaming
#if "TTbarWToLeptonFilter" not in StreamEVGEN.RequireAlgs:
#    StreamEVGEN.RequireAlgs += ["TTbarWToLeptonFilter"]

## Default cut params
filtSeq.TTbarWToLeptonFilter.Ptcut = 1.
