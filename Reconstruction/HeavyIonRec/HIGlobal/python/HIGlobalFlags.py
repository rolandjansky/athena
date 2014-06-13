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

class FlowHarmonicMin(JobProperty):
    """ Min n to include in vn analysis
    """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class FlowHarmonicMax(JobProperty):
    """ Max n to include in vn analysis
    """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 6

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
    FlowHarmonicMin,
    FlowHarmonicMax,
    HIFlowContainerPrefix
    ]

for i in list_jobproperties:
    jobproperties.HIGlobalFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties
