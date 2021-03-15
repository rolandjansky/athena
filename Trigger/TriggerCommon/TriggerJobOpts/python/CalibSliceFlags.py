# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Calib slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

_flags = []
class doTrackingApproach(JobProperty):
    """ Use tracking approach in Calib Trigger """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
_flags.append(doTrackingApproach)

class signatures(JobProperty):
    """ signatures in Calib slice """
    statusOn=True
    allowedTypes = ['list', 'str']
    StoredValue  = []
_flags.append(signatures)


# create container

class CalibSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Calib Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(CalibSlice)

for flag in _flags:
    TriggerFlags.CalibSlice.add_JobProperty(flag)
