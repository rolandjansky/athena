## Example configuration for MultiParticleFilter setting up defaults

if not hasattr( filtSeq, "MultiParticleFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import MultiParticleFilter
    filtSeq += MultiParticleFilter()
    pass

## Setup defaults as example
"""
MultiParticleFilter = filtSeq.MultiParticleFilter
MultiParticleFilter.Np = 1
MultiParticleFilter.ptMinParticle = 105000.
MultiParticleFilter.etaRangeParticle = 4.9
MultiParticleFilter.particlePDG = [11,-11,22]
MultiParticleFilter.particleStatus = [1]
"""

