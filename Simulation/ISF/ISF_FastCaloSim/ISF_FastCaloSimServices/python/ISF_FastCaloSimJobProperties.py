# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file ISF_FastCaloSimJobProperties.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" ISF_FastCaloSimJobProperties
    Python module to hold storegate keys of InDet objects.

"""

__author__ = "KG Tan"
__version__= "$Revision: 476468 $"
__doc__    = "ISF_FastCaloSimJobProperties"

__all__    = [ "ISF_FastCaloSimJobProperties" ]

# kindly stolen from AthenaCommonFlags from S. Binet and M. Gallas

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

##-----------------------------------------------------------------------------
## Define the flag

class FastCaloSimIsActive(JobProperty):
    """Defines whether or not FastCaloSim is being run in the current athena setup"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class RandomStreamName(JobProperty):
    """The random number stream used by FastCaloSim"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'FastCaloSimRnd'

class CaloCellsName(JobProperty):
    """StoreGate collection name for FastCaloSim hits"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'AllCalo'

class FastShowerInputCollection(JobProperty):
    """StoreGate collection name of modified TruthEvent for legayc FastCaloSim use"""
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'TruthEvent'

class DoRandomFluctuations(JobProperty):
    """Determines if Random Fluctuations should be used if particle is a pion"""
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class ParamsInputFilename(JobProperty):
    """ Filename of the input parametrizations file. """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'FastCaloSim/MC16/TFCSparam_v010.root'

##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class ISF_FastCaloSimJobProperties(JobPropertyContainer):
    """Container for the ISF_FastCaloSim key flags
    """
    pass

##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(ISF_FastCaloSimJobProperties)


##-----------------------------------------------------------------------------
## 4th step
## adding the flags to the  container
jobproperties.ISF_FastCaloSimJobProperties.add_JobProperty( FastCaloSimIsActive        )
jobproperties.ISF_FastCaloSimJobProperties.add_JobProperty( RandomStreamName           )
jobproperties.ISF_FastCaloSimJobProperties.add_JobProperty( CaloCellsName              )
jobproperties.ISF_FastCaloSimJobProperties.add_JobProperty( FastShowerInputCollection  )
jobproperties.ISF_FastCaloSimJobProperties.add_JobProperty( DoRandomFluctuations       )
jobproperties.ISF_FastCaloSimJobProperties.add_JobProperty( ParamsInputFilename        )

##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select ISF_FastCaloSimJobProperties as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from ISF_FatrasServices.ISF_FastCaloSimJobProperties import ISF_FastCaloSimFlags
ISF_FastCaloSimFlags = jobproperties.ISF_FastCaloSimJobProperties
