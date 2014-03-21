#
## @file FatrasExample/python/FatrasTuning.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" FatrasKeys
    Python module to hold storegate keys of InDet objects.

"""

__author__ = "A. Salzburger"
__version__= "$Revision: 463109 $"
__doc__    = "FatrasTuning"

__all__    = [ "FatrasTuning" ]

# kindly stolen from AthenaCommonFlags from S. Binet and M. Gallas

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes

class MinimumParticlePtPrimaryTrackCreation(JobProperty):
    """do not simulate primary tracks below this momentum"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 250.

class MinimumMuonPtTrackCreation(JobProperty):
    """Kill the particle when it falls underneath this threshold"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1000.        
    
class MinimumParticleMomentum(JobProperty):
    """Kill the particle when it falls underneath this threshold"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 50.


class MinimumBremPhotonMomentum(JobProperty):
    """Follow the photon if it is over threshold"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 150.

# Conversion Creation: 
class InteractionVolumeRadius(JobProperty):
    """Volume (r) in which brem photons / had int are processed"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1200.
      
class InteractionVolumeHalfZ(JobProperty):
    """Volume (z) in which brem photons / had int are processed"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4000.    
      
## ============================================================================
## Energy Loss through: Bethe-Heitler distribution 
class BetheHeitlerScalor(JobProperty):
    """Scalor to adapt Bethe-Heitler contribution"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1.
    
## ============================================================================
## Multiple Scattering: Gaussian mixture model 
class GaussianMixtureModel(JobProperty):
    """Gaussian mixture model on/off for Multiple Scattering"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

## ============================================================================
## Conversion Creation: 
class ParticleDecayMinChildEnergy(JobProperty):
    """minimum child energy from a conversion"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 250.
        
## ============================================================================
## Conversion Creation: 
class ConversionMinChildEnergy(JobProperty):
    """minimum child energy from a conversion"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 100.
    
class ConversionChildEnergyScalor(JobProperty):
    """minimum child energy from a conversion"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 2.
 
# Conversion Creation: 
class ConversionProbabilityScalor(JobProperty):
    """Scalor to adapt the conversion probability"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.98
    
# Conversion Creation: 
class ConversionVolumeRadius(JobProperty):
    """Volume (r) in which conversions are created"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1200.
      
class ConversionVolumeHalfZ(JobProperty):
    """Volume (z) in which conversions are created"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 4000.

    
## ============================================================================
## Hadronic interactions:     
class HadronicInteractionProbabilityScalor(JobProperty):
    """ """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1.

## Hadronic interactions:     
class HadronicInteractionMinMomentumIn(JobProperty):
    """ """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1000.
    
## Hadronic interactions:     
class HadronicInteractionMinMomentumOut(JobProperty):
    """Volume (z) in which conversions are created"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 200.    
           

## ============================================================================
## Noise Levels
class PixNoiseLevel(JobProperty):
    """Pixel overall noise level (fraction of noisy pixels)"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1e-5

class SctNoiseLevel(JobProperty):
    """SCT overall noise level (fraction of noisy strips)"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1e-5

class TrtNoiseLevel(JobProperty):
    """TRT overall noise level (fraction of noisy straws)"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.02

class TrtMultiHitFlag(JobProperty):
    """Flag how to deal with TRT multiple hits on straw"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

# Material distortions
class MaterialScalor(JobProperty):    
    """efficiencies (vs. eta) for the TRT"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1.00

class MaterialDistortionsX0(JobProperty):    
    """efficiencies (vs. eta) for the TRT"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.02
    
# Material distortions
class MaterialDistortionsRho(JobProperty):    
    """efficiencies (vs. eta) for the TRT"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.02
    
##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class FatrasTuning(JobPropertyContainer):
    """Container for the Fatras key flags
    """
    pass


##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(FatrasTuning)


##-----------------------------------------------------------------------------
## 4th step
## adding ID flags to the InDetKeys container
jobproperties.FatrasTuning.add_JobProperty(MinimumParticlePtPrimaryTrackCreation)
jobproperties.FatrasTuning.add_JobProperty(MinimumMuonPtTrackCreation)
jobproperties.FatrasTuning.add_JobProperty(MinimumParticleMomentum)
jobproperties.FatrasTuning.add_JobProperty(MinimumBremPhotonMomentum)
jobproperties.FatrasTuning.add_JobProperty(InteractionVolumeRadius)
jobproperties.FatrasTuning.add_JobProperty(InteractionVolumeHalfZ)
jobproperties.FatrasTuning.add_JobProperty(BetheHeitlerScalor)
jobproperties.FatrasTuning.add_JobProperty(GaussianMixtureModel)
jobproperties.FatrasTuning.add_JobProperty(ParticleDecayMinChildEnergy)
jobproperties.FatrasTuning.add_JobProperty(ConversionMinChildEnergy)
jobproperties.FatrasTuning.add_JobProperty(ConversionChildEnergyScalor)
jobproperties.FatrasTuning.add_JobProperty(ConversionProbabilityScalor)
jobproperties.FatrasTuning.add_JobProperty(ConversionVolumeRadius)
jobproperties.FatrasTuning.add_JobProperty(ConversionVolumeHalfZ)     
jobproperties.FatrasTuning.add_JobProperty(HadronicInteractionProbabilityScalor)
jobproperties.FatrasTuning.add_JobProperty(HadronicInteractionMinMomentumIn)
jobproperties.FatrasTuning.add_JobProperty(HadronicInteractionMinMomentumOut)
jobproperties.FatrasTuning.add_JobProperty(PixNoiseLevel)
jobproperties.FatrasTuning.add_JobProperty(SctNoiseLevel)
jobproperties.FatrasTuning.add_JobProperty(TrtNoiseLevel)
jobproperties.FatrasTuning.add_JobProperty(TrtMultiHitFlag)
jobproperties.FatrasTuning.add_JobProperty(MaterialScalor)
jobproperties.FatrasTuning.add_JobProperty(MaterialDistortionsX0)
jobproperties.FatrasTuning.add_JobProperty(MaterialDistortionsRho)
##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select FatrasKeys as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from FatrasExample.FatrasTuning import FatrasTuningFlags
FatrasTuningFlags = jobproperties.FatrasTuning
