# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

""" Combined slices specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties # noqa: F401
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

__doc__="Flags for combined slices  "


_flags = [] 

class signatures(JobProperty):
    """ signatures in Combined slice """
    statusOn=True
    allowedTypes=['list']    
    StoredValue = []
    
_flags.append(signatures)

# create container
class CombinedSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Combined Slices Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(CombinedSlice)

for flag in _flags:
    TriggerFlags.CombinedSlice.add_JobProperty(flag)
del _flags

# make an alias
CombinedSliceFlags = TriggerFlags.CombinedSlice



