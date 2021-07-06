# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" MET slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

_flags = []

class signatures(JobProperty):
    """ signatures in MET slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []

_flags.append(signatures)


class METSlice(JobPropertyContainer, CommonSignatureHelper):
    """ MET Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(METSlice)


for flag in _flags:
    TriggerFlags.METSlice.add_JobProperty(flag)
