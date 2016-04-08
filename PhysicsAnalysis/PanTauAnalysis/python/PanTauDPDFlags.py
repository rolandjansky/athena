# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
## @file PanTauAnalysis/python/PanTauDPDFlags.py
## @purpose Python module to hold common flags to configure PanTau DPD writing
##

""" PanTauDPDFlags
    Python module to hold flags to configure PanTau DPD writing.

    From the python prompt:
    >>> from PanTauAnalysis.PanTauDPDFlags import panTauDPDFlags

"""

__author__ = "S.Fleischmann"
__version__= "$Revision: 630996 $"
__doc__    = "PanTauDPDFlags"

__all__    = [ "panTauDPDFlags" ]

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes
class WritePanTauPerfD3PD(JobProperty):
    """ Flag to switch on/off PanTauPerf writing """
    statusON     = True
    allowedTypes = ['bool']
    StoredValue  = False

class PanTauPerfD3PDName(JobProperty):
    """ Name of pantau D3PD if it is created """
    statusON     = True
    allowedTypes = ['str']
    StoredValue  = 'PanTauPerf'

class PanTauPerfD3PDSize(JobProperty):
    """ type of pantau D3PD if it is created (small / medium)"""
    statusON     = True
    allowedTypes = ['str']
    StoredValue  = 'small'


class GoodRunsListVec(JobProperty):
    """Event Selection: List of good runs list files"""
    statusON     = True
    allowedType  = ['list']
    StoredValue  = [ "MyLBCollection.xml" ]

class GoodRunsListArray(JobProperty):
    """Event Selection: List of good runs list to use"""
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['MyLBCollection']

class UseGRL(JobProperty):
    """Event Selection: Use GRL in event selection?"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class TriggerSelectionExpression(JobProperty):
    """Event Selection: Expression of trigger items to use in event selection"""
    statusON     = True
    allowedTypes = ['str']
    StoredValue  = 'L1_J5'

class FilterEvents(JobProperty):
    """Event Selection: Do event selection in PanTau perf DPD making?"""
    statusON     = True
    allowedTypes = ['bool']
    StoredValue  = True

class EventSelTauSGKey(JobProperty):
    """Event Selection: Key of tau SG container"""
    statusON     = True
    allowedTypes = ['str']
    StoredValue  = 'TauJets'

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the PanTauDPD flag container
class PanTauDPDFlags(JobPropertyContainer):
    """Container for the common flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(PanTauDPDFlags)

##-----------------------------------------------------------------------------
## 4th step
## adding athena common flags to the AthenaCommonFlags container
jobproperties.PanTauDPDFlags.add_JobProperty(GoodRunsListVec)
jobproperties.PanTauDPDFlags.add_JobProperty(GoodRunsListArray)
jobproperties.PanTauDPDFlags.add_JobProperty(UseGRL)
jobproperties.PanTauDPDFlags.add_JobProperty(TriggerSelectionExpression)
jobproperties.PanTauDPDFlags.add_JobProperty(FilterEvents)
jobproperties.PanTauDPDFlags.add_JobProperty(EventSelTauSGKey)
jobproperties.PanTauDPDFlags.add_JobProperty(WritePanTauPerfD3PD)


# Helper to create a boolean property.
def _boolean_prop (name, val):
    prop = type (name, (JobProperty,), {})
    prop.statusOn = True
    prop.allowedTypes = ['bool']
    prop.StoredValue = val
    jobproperties.PanTauDPDFlags.add_JobProperty(prop)
    return

_boolean_prop ('UseTriggerSelection', True)
_boolean_prop ('UseJetCleaning', True)
_boolean_prop ('UsePrimaryVertexSelection', True)
#_boolean_prop ('UseTauSelection', True)
_boolean_prop ('UseDiTauSelection', True)

def _sgkey_prop (name, val):
    prop = type (name, (JobProperty,), {})
    prop.statusOn = True
    prop.allowedTypes = ['str']
    prop.StoredValue = val
    jobproperties.PanTauDPDFlags.add_JobProperty(prop)
    return

_sgkey_prop("PanTauPerfD3PDName", "PanTauPerf")
_sgkey_prop("PanTauPerfD3PDSize", "tiny")
_sgkey_prop("EventSelectionDecisionContainerName", "pantau_perf_SkimDecisionsContainer")

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select PanTauFlags as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from PanTauAnalysis.PanTauDPDFlags import panTauDPDFlags
panTauDPDFlags = jobproperties.PanTauDPDFlags
