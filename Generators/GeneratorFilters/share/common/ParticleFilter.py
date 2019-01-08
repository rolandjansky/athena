if not hasattr(filtSeq, "ParticleFilter"):
    from GeneratorFilters.GeneratorFiltersConf import ParticleFilter
    filtSeq += ParticleFilter("ParticleFilter")

# Example usage of this filter
#ParticleFilter = filtSeq.ParticleFilter
#ParticleFilter.Ptcut = 0.0
#ParticleFilter.Etacut = 10.0
#ParticleFilter.PDG = 1000022
#ParticleFilter.MinParts = 2
#ParticleFilter.StatusReq = 11
