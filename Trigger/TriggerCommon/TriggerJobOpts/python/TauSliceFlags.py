# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Tau slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

_flags = []

class doTrackingApproach(JobProperty):
    """ Use tracking approach in Tau Trigger """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
_flags.append(doTrackingApproach)

class signatures(JobProperty):
    """ signatures in Tau slice """
    statusOn=True
    allowedTypes = ['list', 'str']
    StoredValue = []

_flags.append(signatures)



# create container

class TauSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Tau Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(TauSlice)

for flag in _flags:
    TriggerFlags.TauSlice.add_JobProperty(flag)
