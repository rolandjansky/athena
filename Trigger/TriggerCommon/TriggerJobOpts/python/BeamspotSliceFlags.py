# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

""" Beamspot slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

__doc__="Beamspot slice specific flags  "

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

# add add common slice flags
#TriggerFlags.BeamspotSlice.import_JobProperties('TriggerJobOpts.CommonSignatureFlags')

for flag in _flags:
    TriggerFlags.BeamspotSlice.add_JobProperty(flag)
del _flags

# make an alias
BeamspotSliceFlags = TriggerFlags.BeamspotSlice
