# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" EnhancedBias slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

_flags = []

class SliceList(JobProperty):
    """ List of chains provided by the EnhancedBias slice """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
_flags.append(SliceList)

class signatures(JobProperty):
    """ signatures in EnhancedBias slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue= []
_flags.append(signatures)

# create container

class EnhancedBiasSlice(JobPropertyContainer, CommonSignatureHelper):
    """ EnhancedBias Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(EnhancedBiasSlice)

for flag in _flags:
    TriggerFlags.EnhancedBiasSlice.add_JobProperty(flag)
