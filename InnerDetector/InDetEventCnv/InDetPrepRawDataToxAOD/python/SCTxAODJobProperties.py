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
    """ Default prescale factor for SCT AxAOD """
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 1
    pass
jobproperties.SCTxAODJobPropertyContainer.add_JobProperty(Prescale)

class PrescaleExpress(JobProperty):
    """ Prescale factor for express stream for SCT AxAOD """
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 50
    pass
jobproperties.SCTxAODJobPropertyContainer.add_JobProperty(PrescaleExpress)

class PrescaleIDprescaledL1(JobProperty):
    """ Prescale factor for IDprescaledL1 stream for SCT AxAOD """
    statusOn = True
    allowedTypes = ['int']
    StoredValue = 10
    pass
jobproperties.SCTxAODJobPropertyContainer.add_JobProperty(PrescaleIDprescaledL1)

SCTxAODFlags = jobproperties.SCTxAODJobPropertyContainer
