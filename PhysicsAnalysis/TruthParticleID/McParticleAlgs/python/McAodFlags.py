# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: McAodFlags.py
# @purpose: a container of flags for McAod
# @author: Sebastien Binet <binet@cern.ch>

"""  A container of flags for McAod

"""
#
#
__author__  = 'Sebastien Binet'
__version__ = "$Revision: 1.1 $"
__doc__     = "A container of flags for McAod"

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

#
class doTruthEtIsolations(JobProperty):
    """ Flag to activate the computation of Et isolations for TruthParticles
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

# Defines the container for the McAod flags
class McAodFlags(JobPropertyContainer):
    """ The global McAod flag/job property container.
    """
    pass

# add the McAod flags container to the top container 
jobproperties.add_Container(McAodFlags)

# We want always the following flags in the container  
list_jobproperties = [
    doTruthEtIsolations,
    ]

for i in list_jobproperties:
    jobproperties.McAodFlags.add_JobProperty(i)

## module clean-up
del list_jobproperties
