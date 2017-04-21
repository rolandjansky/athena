# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" SLHC_JobProperties
    Python module to hold storegate keys of InDet objects.
"""

__author__ =   "A. Salzburger"
__version__=   "$Revision: 1.13 $"
__doc__    =   "SLHC_JobProperties"
__all__    = [ "SLHC_JobProperties" ]

# kindly stolen from AthenaCommonFlags from S. Binet and M. Gallas

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes
class SLHC_Version(JobProperty):
    """Steering of SLHC jobs"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'None'

class LocalDictionaryBaseName(JobProperty):
    """Set the local geometry db name"""
    statusOn     = True
    allowedTypes = [ 'str' ]
    StoredValue  = 'IdDictInnerDetector_'


class LocalGeometryDbEnvName(JobProperty):
    """Set the local geometry db name"""
    statusOn     = True
    allowedTypes = [ 'str' ]
    StoredValue  = 'LocalSlhcGeometryDatabase'
    
class LocalGeometryDbBaseName(JobProperty):
    """Set the local geometry db name"""
    statusOn     = True
    allowedTypes = [ 'str' ]
    StoredValue  = 'slhcsct_local_database_'
    
class InputGeometryPath(JobProperty):
    """Steering of SLHC jobs, getting the database and the dictionary"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ''

class doGMX(JobProperty):
    """Steering of SLHC jobs, getting the database and the dictionary"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class LayoutOption(JobProperty):
    """Steering of SLHC jobs"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = ''

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the flag container
class SLHC_JobProperties(JobPropertyContainer):
    """Container for the Fatras key flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(SLHC_JobProperties)

##-----------------------------------------------------------------------------
## 4th step
## adding ID flags to the InDetKeys container
jobproperties.SLHC_JobProperties.add_JobProperty(SLHC_Version)
jobproperties.SLHC_JobProperties.add_JobProperty(InputGeometryPath)
jobproperties.SLHC_JobProperties.add_JobProperty(LocalDictionaryBaseName)
jobproperties.SLHC_JobProperties.add_JobProperty(LocalGeometryDbEnvName)
jobproperties.SLHC_JobProperties.add_JobProperty(LocalGeometryDbBaseName)
jobproperties.SLHC_JobProperties.add_JobProperty(doGMX)
jobproperties.SLHC_JobProperties.add_JobProperty(LayoutOption)
##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select FatrasJobProperties as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags = jobproperties.SLHC_JobProperties
