# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file ISF_FatrasJobProperties.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" ISF_FatrasJobProperties
    Python module to hold storegate keys of InDet objects.

"""

__author__ = "A. Salzburger"
__version__= "$Revision: 476468 $"
__doc__    = "FatrasJobProperties"

__all__    = [ "ISF_FatrasJobProperties" ]

# kindly stolen from AthenaCommonFlags from S. Binet and M. Gallas

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step
## Define the flag
class OutputLevelGeneral(JobProperty):
    """The output level for Tools"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3

class OutputLevelToolsID(JobProperty):
    """The output level for Tools"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3

class OutputLevelToolsMS(JobProperty):
    """The output level for Tools"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3

class OutputLevelToolsG4(JobProperty):
    """The output level for Tools"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3

class RandomStreamName(JobProperty):
    """The output level for Tools"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'FatrasRnd'

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class ISF_FatrasJobProperties(JobPropertyContainer):
    """Container for the ISF_Fatras key flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(ISF_FatrasJobProperties)


##-----------------------------------------------------------------------------
## 4th step
## adding the flags to the  container
jobproperties.ISF_FatrasJobProperties.add_JobProperty(OutputLevelGeneral)
jobproperties.ISF_FatrasJobProperties.add_JobProperty(OutputLevelToolsID)
jobproperties.ISF_FatrasJobProperties.add_JobProperty(OutputLevelToolsMS)
jobproperties.ISF_FatrasJobProperties.add_JobProperty(OutputLevelToolsG4)
jobproperties.ISF_FatrasJobProperties.add_JobProperty(RandomStreamName)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select FatrasJobProperties as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from ISF_FatrasServices.ISF_FatrasJobProperties import ISF_FatrasFlags
ISF_FatrasFlags = jobproperties.ISF_FatrasJobProperties
