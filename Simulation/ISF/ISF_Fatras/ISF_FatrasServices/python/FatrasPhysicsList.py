# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file FatrasExample/python/FatrasPhysicsList.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" FatrasPhysicsList
    Python module to hold storegate keys of InDet objects.

"""

__author__ = "A. Salzburger"
__version__= "$Revision: 476468 $"
__doc__    = "FatrasPhysicsList"

__all__    = [ "FatrasPhysicsList" ]

# kindly stolen from AthenaCommonFlags from S. Binet and M. Gallas

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes
class MultipleScattering(JobProperty):
    """Steering of multiple scattering in simulation/reconstruction"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class EnergyLoss(JobProperty):
    """Steering of energy loss (ionisation) in simulation/reconstruction"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class Bremsstrahlung(JobProperty):
    """Steering of energy loss (radiative) in simulation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class PairProduction(JobProperty):
    """Steering of pair production in simulation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class HadronicInteraction(JobProperty):
    """Steering of nuclear (hadronic) interactions in simulation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    
class ParticleDecay(JobProperty):
    """Steering of particle decay in simulation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class FatrasPhysicsList(JobPropertyContainer):
    """Container for the Fatras physics list flags
    """
    pass


##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(FatrasPhysicsList)

##-----------------------------------------------------------------------------
## 4th step
## adding ID flags to the InDetKeys container
jobproperties.FatrasPhysicsList.add_JobProperty(MultipleScattering)
jobproperties.FatrasPhysicsList.add_JobProperty(EnergyLoss)
jobproperties.FatrasPhysicsList.add_JobProperty(Bremsstrahlung)
jobproperties.FatrasPhysicsList.add_JobProperty(PairProduction)
jobproperties.FatrasPhysicsList.add_JobProperty(HadronicInteraction)
jobproperties.FatrasPhysicsList.add_JobProperty(ParticleDecay)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select FatrasPhysicsList as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from FatrasExample.FatrasPhysicsList import FatrasPhysicsListFlags
FatrasPhysicsListFlags = jobproperties.FatrasPhysicsList
