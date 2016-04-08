# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file PanTauAnalysis/python/PanTauValidationFlags.py
## @purpose Python module to hold common flags to configure PanTau validation
##

""" PanTauValidationFlags
    Python module to hold flags to configure PanTau.

    From the python prompt:
    >>> from PanTauAnalysis.PanTauValidationFlags import panTauValidationFlags

"""

__author__ = "S.Fleischmann"
__version__= "$Revision: 545441 $"
__doc__    = "PanTauValidationFlags"

__all__    = [ "panTauValidationFlags" ]

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes

class DoSeedEfficiency(JobProperty):
    """Seed validation: Call Seed Efficiency Alg?"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class DoTauEfficiency(JobProperty):
    """Tau Efficiency: Call Tau Efficiency Alg"""
    statusON     = True
    allowedTypes = ['bool']
    StoredValue  = True

class DoFeatureWriter(JobProperty):
    """Feature Writer: Call Feature Writer?"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class DoFeatureDefaultSmearing(JobProperty):
    """Feature Writer: Smear default values?"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class WriteEfoMomentaInFeatureWriter(JobProperty):
    """Feature Writer: Write EFO 4-momenta?"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class WriteMissingSeedsInFeatureWriter(JobProperty):
    """Feature Writer: Write missing seeds?"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class WriteRejectedSeedsInFeatureWriter(JobProperty):
    """Feature Writer: Write missing seeds?"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class DoSubClusterEfficiency(JobProperty):
    """Seed validation: Run sub-cluster efficiency?"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class AllFeatureNames(JobProperty):
    """feature validation: List of all feature Names"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ ]

class AllFeatureDefaultValues(JobProperty):
    """feature validation: List of all feature default values, used if feature is not set (same dimension as AllFeatureNames)"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ ]

class AllFeatureDefaultValueSmearingSigma(JobProperty):
    """feature validation: List of all feature sigmas for default value smearing, used if feature is not set (same dimension as AllFeatureNames)"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ ]

class AllFeatureTypes(JobProperty):
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = [ ]

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the PanTau flag container
class PanTauValidationFlags(JobPropertyContainer):
    """Container for the common flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(PanTauValidationFlags)

##-----------------------------------------------------------------------------
## 4th step
## adding athena common flags to the AthenaCommonFlags container
jobproperties.PanTauValidationFlags.add_JobProperty(DoSeedEfficiency)
jobproperties.PanTauValidationFlags.add_JobProperty(DoTauEfficiency)
jobproperties.PanTauValidationFlags.add_JobProperty(DoFeatureWriter)
jobproperties.PanTauValidationFlags.add_JobProperty(DoFeatureDefaultSmearing)
jobproperties.PanTauValidationFlags.add_JobProperty(WriteEfoMomentaInFeatureWriter)
jobproperties.PanTauValidationFlags.add_JobProperty(WriteMissingSeedsInFeatureWriter)
jobproperties.PanTauValidationFlags.add_JobProperty(WriteRejectedSeedsInFeatureWriter)
jobproperties.PanTauValidationFlags.add_JobProperty(DoSubClusterEfficiency)
jobproperties.PanTauValidationFlags.add_JobProperty(AllFeatureNames)
jobproperties.PanTauValidationFlags.add_JobProperty(AllFeatureDefaultValues)
jobproperties.PanTauValidationFlags.add_JobProperty(AllFeatureDefaultValueSmearingSigma)
jobproperties.PanTauValidationFlags.add_JobProperty(AllFeatureTypes)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select PanTauFlags as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from PanTauAnalysis.PanTauValidationFlags import panTauValidationFlags
panTauValidationFlags = jobproperties.PanTauValidationFlags
