# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Jet slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties # noqa: F401
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

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


for flag in _flags:
    TriggerFlags.JetSlice.add_JobProperty(flag)

