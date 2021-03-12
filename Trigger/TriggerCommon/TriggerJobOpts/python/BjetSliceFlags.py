# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Bjet slice specific flags  """

from AthenaCommon.JobProperties         import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

_flags = []
class signatures(JobProperty):
    """ signatures in Bjet slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []
    
_flags.append(signatures)



# create container

class BjetSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Bjet Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(BjetSlice)

for flag in _flags:
    TriggerFlags.BjetSlice.add_JobProperty(flag)
