# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""  A container of flags for heavy ion derivations

"""
#
#
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class isSimulation(JobProperty):
    """ 
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class isPP(JobProperty):
    """ 
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class isPPb(JobProperty):
    """ 
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class doMinBiasSelection(JobProperty):
    """ 
    """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False


class HIDerivationFlags(JobPropertyContainer):
    """ The HIDerivation property container.
    """
    pass

jobproperties.add_Container(HIDerivationFlags)
list_jobproperties = [isSimulation,isPP,isPPb,doMinBiasSelection]
for i in list_jobproperties:
    jobproperties.HIDerivationFlags.add_JobProperty(i)

del list_jobproperties
HIDerivationFlags=jobproperties.HIDerivationFlags

