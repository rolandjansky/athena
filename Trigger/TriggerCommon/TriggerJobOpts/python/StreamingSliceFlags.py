# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

""" Streaming slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper, AllowedList

__doc__="Streaming slice specific flags  "


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

# add add common slice flags
#TriggerFlags.StreamingSlice.import_JobProperties('TriggerJobOpts.CommonSignatureFlags')

for flag in _flags:
    TriggerFlags.StreamingSlice.add_JobProperty(flag)
del _flags

# make an alias
StreamingSliceFlags = TriggerFlags.StreamingSlice

