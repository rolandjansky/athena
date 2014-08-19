# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: RecConfFlags.py
# @purpose: a container of flags for Configured objects
# @author: Sebastien Binet <binet@cern.ch>

"""  A container of flags for Configured objects

"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.3 $"
__doc__     = "A container of flags for Configured objects"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#
class AllowIgnoreExistingDataObject(JobProperty):
    """Allow an algorithm to ignore wether an input object is available or not.
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

#
class AllowIgnoreConfigError(JobProperty):
    """Allow an algorithm to ignore return error code from upstream algorithm
    and tools.
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

#
class AllowDisable(JobProperty):
    """ Configures the global lock state to allow or not a given 'Configured'
    to be removed from the sequence of algorithms.
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

#
class AllowBackNavigation(JobProperty):
    """ Configures the global lock state to allow or not Back Navigation.
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

# Defines the container for the performance monitoring flags  
class RecConfFlags(JobPropertyContainer):
    """ The global Configured flag/job property container.
    """
    pass

# add the 'Configured' flags container to the top container 
jobproperties.add_Container(RecConfFlags)


# We want always the following flags in the container  
list_jobproperties = [
    AllowIgnoreExistingDataObject,
    AllowIgnoreConfigError,
    AllowDisable,
    AllowBackNavigation
    ]

for i in list_jobproperties:
    jobproperties.RecConfFlags.add_JobProperty(i)

recConfFlags=jobproperties.RecConfFlags

## module clean-up
del list_jobproperties

