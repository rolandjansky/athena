# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file InDetPrepRawDataToxAOD/python/SCTxAODJobProperties.py
## @purpose Python module to hold common flags to configure SCT AxAOD-making jobOptions
##

""" SCTxAODJobProperties
    Python module to hold common flags to configure SCT AxAOD-making jobOptions.

"""


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

class SCTxAODJobPropertyContainer(JobPropertyContainer):
    """ The jobProperty container for SCT AxAOD-making flags """
jobproperties.add_Container(SCTxAODJobPropertyContainer)

class Prescale(JobProperty):
    """ Prescale factor for SCT AxAOD """
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 1
    pass
jobproperties.SCTxAODJobPropertyContainer.add_JobProperty(Prescale)

SCTxAODFlags = jobproperties.SCTxAODJobPropertyContainer
