# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: HIGlobalFlags.py
# @purpose: a container of flags for Heavy Ion Reconstruction of Global variables
# @author: Andrzej Olszewski <Andrzej.Olszewski@ifj.edu.pl>

"""  A container of flags for heavy ion global reconstruction  

"""
#
#
__author__  = 'Andrzej Olszewski'
__version__ = "$Revision: 1.3 $"
__doc__     = "A container of flags for Heavy Ion Reconstruction of Globals"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class doHIGlobalVars(JobProperty):
    """ Run heavy ion global variable reconstruction 
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doHIFlow(JobProperty):
    """ Switch for HIFlow
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class NumFlowHarmonics(JobProperty):
    """ Min n to include in vn analysis
    """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 7

class HIFlowContainerPrefix(JobProperty):
    """ Max n to include in vn analysis
    """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'HIFlowData_v'



    
class doHICentrality(JobProperty):
    """ Switch for HICentrality
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doHITRT(JobProperty):
    """ Switch for HITRT
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doHIPixelTracklet(JobProperty):
    """ Run heavy ion pixel tracklet reconstruction 
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class doHIGlobalNSiCluster(JobProperty):
    """ Switch for HIGlobalNSiCluster
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class EventShapeKey(JobProperty):
    """ Name of HIEventShape object container
    """
    statusOn     = True
    allowedTypes = ['string']
    StoredValue  = "HIEventShape"

class EventShapeSummaryKey(JobProperty):
    """ Name of HIEventShape object container containing summary info
    """
    statusOn     = True
    allowedTypes = ['string']
    StoredValue  = 'CaloSums'

class SummarySubCalos(JobProperty):
    """ List of SubCalos to be written in summary
    """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['FCal','EMCal','HCal','ALL']

class SummarySamplings(JobProperty):
    """ List of samplings to be written in summary
    """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = ['FCAL0','FCAL1','FCAL2']

class DoSummary(JobProperty):
    """ Add extra HIEventShapeContainer containing summary
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class SummaryPN(JobProperty):
    """ Compute separate summaries for positive and negative eta
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class HIGlobalItemList(JobProperty):
    """ List of items to be written to AOD, initially empty but filled based on configuration
    """
    statusOn     = True
    allowedTypes = ['list']
    StoredValue  = []

    
# Defines the container for heavy ion reco flags  
class HIGlobalFlags(JobPropertyContainer):
    """ The global heavy ion reconstruction flag/job property container.
    """
    pass

# add the flags container to the top container 
jobproperties.add_Container(HIGlobalFlags)

# We want always the following flags in the container  
list_jobproperties = [
    doHIGlobalVars,
    doHIFlow,
    doHICentrality,
    doHITRT,
    doHIPixelTracklet,
    doHIGlobalNSiCluster,
    NumFlowHarmonics,
    HIFlowContainerPrefix,
    EventShapeKey,
    DoSummary,
    EventShapeSummaryKey,
    SummarySubCalos,
    SummarySamplings,
    SummaryPN,
    HIGlobalItemList
    ]

for i in list_jobproperties:
    jobproperties.HIGlobalFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties
