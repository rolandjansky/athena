## Instantiate the TTbarWithJpsimumuFilter, including adding it to the stream requirement
## Filter passed if top, antitop and t->b->J/psi->mumu in truth chain
## Configuration of the filter cuts is left to the specific JO

if not hasattr(filtSeq, "TTbarWithJpsimumuFilter"):
    from GeneratorFilters.GeneratorFiltersConf import TTbarWithJpsimumuFilter
    filtSeq += TTbarWithJpsimumuFilter()

## Default cut params
filtSeq.TTbarWithJpsimumuFilter.JpsipTMinCut = 5000.