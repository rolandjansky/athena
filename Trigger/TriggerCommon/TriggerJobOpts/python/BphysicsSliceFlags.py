# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

""" Bphysics slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSignatureHelper

_flags = []

class signatures(JobProperty):
    """ signatures in Bphysics slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []

_flags.append(signatures)



# create container

class BphysicsSlice(JobPropertyContainer, CommonSignatureHelper):
    """ Bphysics Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(BphysicsSlice)

for flag in _flags:
    TriggerFlags.BphysicsSlice.add_JobProperty(flag)
