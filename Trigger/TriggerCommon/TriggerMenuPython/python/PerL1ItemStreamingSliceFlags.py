# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Streaming slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'W. Fedorko'
__version__="$Revision: 276009 $"
__doc__="PerL1ItemStreaming slice specific flags  "


_flags = []


class signatures(JobProperty):
    """ signatures in PerL1ItemStreaming slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue= []
    allowedValues = AllowedList(StoredValue)
_flags.append(signatures)

# create container
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper

class PerL1ItemStreamingSlice(JobPropertyContainer, CommonSliceHelper):
    """ PerL1ItemStreaming Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(PerL1ItemStreamingSlice)

# add add common slice flags
TriggerFlags.PerL1ItemStreamingSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.PerL1ItemStreamingSlice.add_JobProperty(flag)
del _flags

# make an alias
PerL1ItemStreamingSliceFlags = TriggerFlags.PerL1ItemStreamingSlice

