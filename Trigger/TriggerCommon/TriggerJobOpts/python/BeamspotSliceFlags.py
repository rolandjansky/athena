# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Beamspot slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

_flags = []

class signatures(JobProperty):
    """ signatures in Beamspot slice """
    statusOn=True
    allowedTypes = ['list', 'str']
    StoredValue  = []
_flags.append(signatures)


# create container

class BeamspotSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Beamspot Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(BeamspotSlice)

for flag in _flags:
    TriggerFlags.BeamspotSlice.add_JobProperty(flag)
