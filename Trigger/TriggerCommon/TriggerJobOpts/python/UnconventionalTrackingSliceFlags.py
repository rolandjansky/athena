# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

""" UnconventionalTracking slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties # noqa: F401
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

__doc__="UnconventionalTracking slice specific flags  "


_flags = [] 


class SliceList(JobProperty):
    """ List of chains provided by the Unconventional Tracking slice """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
_flags.append(SliceList)


class signatures(JobProperty):
    """ signatures in UnconventionalTracking slice """
    statusOn=True
    allowedTypes = ['list']
    StoredValue  = []
_flags.append(signatures)


class UnconventionalTrackingSlice(JobPropertyContainer, CommonSignatureHelper):
    """ UnconventionalTracking Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(UnconventionalTrackingSlice)

# add add common slice flags
#TriggerFlags.UnconventionalTrackingSlice.import_JobProperties('TriggerJobOpts.CommonSignatureFlags')

for flag in _flags:
    TriggerFlags.UnconventionalTrackingSlice.add_JobProperty(flag)
del _flags

# make an alias
UnconventionalTrackingSliceFlags = TriggerFlags.UnconventionalTrackingSlice
