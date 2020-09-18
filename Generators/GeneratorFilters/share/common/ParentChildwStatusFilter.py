if not hasattr(filtSeq, "ParentChildwStatusFilter"):
    from GeneratorFilters.GeneratorFiltersConf import ParentChildwStatusFilter
    filtSeq += ParentChildwStatusFilter("ParentChildwStatusFilter")

## Example usage for Z leptonic decay from Pythia8
#filtSeq.ParentChildwStatusFilter.PDGParent  = [23]
#filtSeq.ParentChildwStatusFilter.StatusParent = [22,52]
#filtSeq.ParentChildwStatusFilter.PtMinParent =  -1e12
#filtSeq.ParentChildwStatusFilter.PtMaxParent = 1e12
#filtSeq.ParentChildwStatusFilter.EtaRangeParent = 1e12
#filtSeq.ParentChildwStatusFilter.MassMinParent = -1e12
#filtSeq.ParentChildwStatusFilter.MassMaxParent = 1e12
#filtSeq.ParentChildwStatusFilter.PDGChild = [11,13,15]
#filtSeq.ParentChildwStatusFilter.PtMinChild = -1e12
#filtSeq.ParentChildwStatusFilter.EtaRangeChild = 1e12
