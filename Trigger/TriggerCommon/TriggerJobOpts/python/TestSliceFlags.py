# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Test slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

_flags = [] 

class signatures(JobProperty):
    """ signatures in Test slice """
    statusOn=True
    allowedTypes=['list']

    StoredValue   = [
        ]

_flags.append(signatures)

# create container
class TestSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Test Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(TestSlice)

for flag in _flags:
    TriggerFlags.TestSlice.add_JobProperty(flag)
