# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Streaming slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

_flags = []

class SliceList(JobProperty):
    """ List of chains provided by the Streaming slice """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
_flags.append(SliceList)

class signatures(JobProperty):
    """ signatures in Streaming slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue= []
_flags.append(signatures)

# create container

class StreamingSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Streaming Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(StreamingSlice)

for flag in _flags:
    TriggerFlags.StreamingSlice.add_JobProperty(flag)
