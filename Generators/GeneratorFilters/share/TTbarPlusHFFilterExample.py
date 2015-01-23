# Example JO to filter tt+HF samples


if not hasattr(topAlg, "TTbarPlusBFilter"):
    from GeneratorFilters.GeneratorFiltersConf import TTbarPlusHeavyFlavorFilter
    topAlg += TTbarPlusHeavyFlavorFilter("TTbarPlusBFilter")

## Add this filter to the algs required to be successful for streaming
if "TTbarPlusBFilter" not in StreamEVGEN.RequireAlgs:
    StreamEVGEN.RequireAlgs += ["TTbarPlusBFilter"]

## Default cut params
topAlg.TTbarPlusBFilter.SelectB = True  ## for tt+b
## topAlg.TTbarPlusBFilter.SelectC = True ## for tt+c
## topAlg.TTbarPlusBFilter.SelectL = True ## for tt+light




