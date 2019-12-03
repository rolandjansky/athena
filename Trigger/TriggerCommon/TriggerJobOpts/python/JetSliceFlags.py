# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

""" Jet slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties # noqa: F401
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

__doc__="Jet slice specific flags  "


_flags = []

class signatures(JobProperty):
    """ signatures in Jet slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []
    
_flags.append(signatures)


class JetSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Jet Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(JetSlice)

# add add common slice flags
#TriggerFlags.JetSlice.import_JobProperties('TriggerJobOpts.CommonSignatureFlags')

for flag in _flags:
    TriggerFlags.JetSlice.add_JobProperty(flag)
del _flags

# make an alias
JetSliceFlags = TriggerFlags.JetSlice

