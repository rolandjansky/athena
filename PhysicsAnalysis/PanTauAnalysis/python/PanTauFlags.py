# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file PanTauAnalysis/python/PanTauFlags.py
## @purpose Python module to hold common flags to configure PanTau
##

""" PanTauFlags
    Python module to hold flags to configure PanTau.

    From the python prompt:
    >>> from PanTauAnalysis.PanTauFlags import panTauFlags

"""

__author__ = "S.Fleischmann"
__version__= "$Revision: 537172 $"
__doc__    = "PanTauFlags"

__all__    = [ "panTauFlags" ]

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties
import AthenaCommon.SystemOfUnits as Units

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes


### ==========================================================
###
###          Flags to steer job options
###
### ==========================================================

class GetJetFeatures(JobProperty):
    """Add the JetFeatureExtraction tool to the list of tools?"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class GetImpactParameterFeatures(JobProperty):
    """Add the ImpactParameterExtraction tool to the list of tools?"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class DoTauJetBuilding(JobProperty):
    """Add the TauJetBuilderAlg to the list of algorithms?"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doTruthEtIsolations(JobProperty):
    """ Ask Sebastian for its meaning """
    statusOn        = True
    allowedTypes    = ['bool']
    StoredValue     = False

class ExtractAllFeatures(JobProperty):
    """Flag to tell if ALL features should be extracted to D3PD. Is evaluated in tauPerfMedium and TauD3PDMaker"""
    statusOn    = True
    allowedType = ['bool']
    StoredValue = False

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the PanTau flag container
class PanTauFlags(JobPropertyContainer):
    """Container for the common flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(PanTauFlags)

##-----------------------------------------------------------------------------
## 4th step
## adding athena common flags to the AthenaCommonFlags container
jobproperties.PanTauFlags.add_JobProperty(GetJetFeatures)
jobproperties.PanTauFlags.add_JobProperty(GetImpactParameterFeatures)
jobproperties.PanTauFlags.add_JobProperty(DoTauJetBuilding)
jobproperties.PanTauFlags.add_JobProperty(doTruthEtIsolations)
jobproperties.PanTauFlags.add_JobProperty(ExtractAllFeatures)


##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select PanTauFlags as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from PanTauAnalysis.PanTauFlags import panTauFlags
panTauFlags = jobproperties.PanTauFlags
