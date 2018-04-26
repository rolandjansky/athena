# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Streaming slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

__author__  = 'P. Bell, after A. Cerri'
__version__="$Revision: 0.1 $"
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

class StreamingSlice(JobPropertyContainer, CommonSliceHelper):
    """ Streaming Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(StreamingSlice)

# add add common slice flags
TriggerFlags.StreamingSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.StreamingSlice.add_JobProperty(flag)
del _flags

# make an alias
StreamingSliceFlags = TriggerFlags.StreamingSlice

