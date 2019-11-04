# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

""" Flags for monitoring chains """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

_flags = []

class signatures(JobProperty):
    """ signatures in monitoring slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []
_flags.append(signatures)

# create container
class MonitorSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Monitor Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(MonitorSlice)

## add add common slice flags
#TriggerFlags.MonitorSlice.import_JobProperties('TriggerJobOpts.CommonSignatureFlags')

for flag in _flags:
    TriggerFlags.MonitorSlice.add_JobProperty(flag)
del _flags

## make an alias
MonitorSliceFlags = TriggerFlags.MonitorSlice

