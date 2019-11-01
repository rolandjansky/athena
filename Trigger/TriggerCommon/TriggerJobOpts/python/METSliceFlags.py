# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

""" MET slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerJobOpts.CommonSignatureHelper import CommonSliceHelper

# create container
__doc__="MET slice specific flags  "


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

# add add common slice flags
#TriggerFlags.METSlice.import_JobProperties('TriggerMenu.menu.CommonSignatureFlags')

for flag in _flags:
    TriggerFlags.METSlice.add_JobProperty(flag)
del _flags

# make an alias
METSliceFlags = TriggerFlags.METSlice
