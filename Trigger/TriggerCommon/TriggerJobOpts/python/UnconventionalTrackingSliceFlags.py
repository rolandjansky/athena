# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" UnconventionalTracking slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties # noqa: F401
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper


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

for flag in _flags:
    TriggerFlags.UnconventionalTrackingSlice.add_JobProperty(flag)
