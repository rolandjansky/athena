# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file FatrasExample/python/FatrasValidation.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" FatrasValidation
    Python module to hold storegate keys of InDet objects.

"""

__author__ = "A. Salzburger"
__version__= "$Revision: 476468 $"
__doc__    = "FatrasValidation"

__all__    = [ "FatrasValidation" ]

# kindly stolen from AthenaCommonFlags from S. Binet and M. Gallas

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes
class OutputFileName(JobProperty):
    """Steering of output file name for validation"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'FatrasOutput.root'

class ValidationStream(JobProperty):
    """TTree stream to be written at"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'val'

class MaterialEffects(JobProperty):
    """Steering of multiple scattering/eloss (ionisation) in validation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class MaterialDistortions(JobProperty):
    """Steering of material distortions in validation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class Bremsstrahlung(JobProperty):
    """Steering of energy loss (radiative) in validation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class CaloEnergyDeposition(JobProperty):
    """Steering of energy loss (radiative) in validation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    
class PairProduction(JobProperty):
    """Steering of pair production in validation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class HadronicInteractions(JobProperty):
    """Steering of pair production in validation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class ParticleDecay(JobProperty):
    """Steering of particle decay in validation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class PreProcessing(JobProperty):
    """Steering of validation: validation simulation containers"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class Simulation(JobProperty):
    """Steering of validation: validation simulation containers"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class Refitting(JobProperty):
    """Steering of validation: validation refitted track containers"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
        
class TrkNtuple(JobProperty):
    """Steering of validation: validation using trk ntuple"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class Residuals(JobProperty):
    """Steering of validation: validation using residuals for validation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class TrackPlotter(JobProperty):
    """Steering of validation: switch for track plotter"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class McNtuple(JobProperty):
    """Steering of validation: include McNtuple into output ntule"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class PixelClusterSimulation(JobProperty):
    """Steering of validation: PixelClusterSimulation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class SctClusterSimulation(JobProperty):
    """Steering of validation: SCT_ClusterSimulation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class TrtDriftCircleSimulation(JobProperty):
    """Steering of validation: TRT_DriftCircleSimulation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

#
class TrtPostProcessor(JobProperty):
    """Steering of validation: TRT_Processor"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class MuonSimHitNtuple(JobProperty):
    """Steering of validation: MuonHitNtuple"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
class MuonSimHitValidation(JobProperty):
    """Steering of validation: MuonSimHitValidation"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
class MuonMonitoringNtuple(JobProperty):
    """Steering of validation: Muon MonitoringNtuple"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class CheckGenEventRecord(JobProperty):
    """Steering of validation: run FatrasTestHepMC to check GenEvent"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class FatrasValidation(JobPropertyContainer):
    """Container for the Fatras physics list flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(FatrasValidation)

##-----------------------------------------------------------------------------
## 4th step
## adding ID flags to the InDetKeys container
jobproperties.FatrasValidation.add_JobProperty(OutputFileName)
jobproperties.FatrasValidation.add_JobProperty(ValidationStream)
jobproperties.FatrasValidation.add_JobProperty(MaterialEffects)
jobproperties.FatrasValidation.add_JobProperty(MaterialDistortions)
jobproperties.FatrasValidation.add_JobProperty(Bremsstrahlung)
jobproperties.FatrasValidation.add_JobProperty(CaloEnergyDeposition)
jobproperties.FatrasValidation.add_JobProperty(PairProduction)
jobproperties.FatrasValidation.add_JobProperty(HadronicInteractions)
jobproperties.FatrasValidation.add_JobProperty(ParticleDecay)
jobproperties.FatrasValidation.add_JobProperty(PreProcessing)
jobproperties.FatrasValidation.add_JobProperty(Simulation)
jobproperties.FatrasValidation.add_JobProperty(Refitting)
jobproperties.FatrasValidation.add_JobProperty(TrkNtuple)
jobproperties.FatrasValidation.add_JobProperty(PixelClusterSimulation)
jobproperties.FatrasValidation.add_JobProperty(SctClusterSimulation)
jobproperties.FatrasValidation.add_JobProperty(TrtDriftCircleSimulation)
#
jobproperties.FatrasValidation.add_JobProperty(TrtPostProcessor)
jobproperties.FatrasValidation.add_JobProperty(Residuals)
jobproperties.FatrasValidation.add_JobProperty(TrackPlotter)
jobproperties.FatrasValidation.add_JobProperty(McNtuple)
jobproperties.FatrasValidation.add_JobProperty(MuonSimHitNtuple)
jobproperties.FatrasValidation.add_JobProperty(MuonSimHitValidation)
jobproperties.FatrasValidation.add_JobProperty(MuonMonitoringNtuple)
jobproperties.FatrasValidation.add_JobProperty(CheckGenEventRecord)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select FatrasValidation as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from FatrasExample.FatrasValidation import FatrasValidationFlags
FatrasValidationFlags = jobproperties.FatrasValidation
