# common fragment for xAODSameParticleHardScattering filter
# conversion to XAOD, 
# connecting the filter

include ("GeneratorFilters/CreatexAODSlimContainers.py")

from GeneratorFilters.GeneratorFiltersConf import xAODSameParticleHardScatteringFilter
xAODSameParticleHardScatteringFilter = xAODSameParticleHardScatteringFilter("xAODSameParticleHardScatteringFilter")
filtSeq += xAODSameParticleHardScatteringFilter

# to modiify cuts put into JOs e.g.:
#filtSeq.xAODSameParticleHardScatteringFilter.PDGParent = [-5,5]  # Select b-bbar scattering  
#filtSeq.xAODSameParticleHardScatteringFilter.PDGChild = [4000015]   # Select bbar fusion to excited tau 

