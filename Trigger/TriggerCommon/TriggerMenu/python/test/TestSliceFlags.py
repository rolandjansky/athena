# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" MinBias slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

__author__  = 'M. Backes'
__version__=""
__doc__=""

_flags = [] 

class signatures(JobProperty):
    """ signatures in Test slice """
    statusOn=True
    allowedTypes=['list']

    StoredValue   = [
        ]

_flags.append(signatures)

# create container
class TestSlice(JobPropertyContainer, CommonSliceHelper):
    """ Test Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(TestSlice)

# add add common slice flags
TriggerFlags.TestSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.TestSlice.add_JobProperty(flag)
del _flags

# make an alias
TestSliceFlags = TriggerFlags.TestSlice

