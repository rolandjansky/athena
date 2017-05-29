# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Flags for monitoring chains """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

_flags = [] 

class signatures(JobProperty):
    """ signatures in monitoring slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []
_flags.append(signatures)

# create container
class MonitorSlice(JobPropertyContainer, CommonSliceHelper):
    """ Monitor Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(MonitorSlice)

## add add common slice flags
TriggerFlags.MonitorSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.MonitorSlice.add_JobProperty(flag)
del _flags

## make an alias
MonitorSliceFlags = TriggerFlags.MonitorSlice

