# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file ISF_ParSimJobProperties.py
## @purpose Python module to hold common flags to configure JobOptions
##

__author__ = "Miha Muskinja"
__version__= "$Revision: 698109 $"
__doc__    = "ParSimJobProperties"

__all__    = [ "ISF_ParSimJobProperties" ]

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

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class ISF_ParSimJobProperties(JobPropertyContainer):
    """Container for the ISF_ParSim key flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(ISF_ParSimJobProperties)


##-----------------------------------------------------------------------------
## 4th step
## adding the flags to the  container
jobproperties.ISF_ParSimJobProperties.add_JobProperty(OutputLevelGeneral)
jobproperties.ISF_ParSimJobProperties.add_JobProperty(OutputLevelToolsID)
jobproperties.ISF_ParSimJobProperties.add_JobProperty(OutputLevelToolsMS)
jobproperties.ISF_ParSimJobProperties.add_JobProperty(OutputLevelToolsG4)

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select ParSimJobProperties as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from ISF_ParSimServices.ISF_ParSimJobProperties import ISF_ParSimFlags
ISF_ParSimFlags = jobproperties.ISF_ParSimJobProperties
