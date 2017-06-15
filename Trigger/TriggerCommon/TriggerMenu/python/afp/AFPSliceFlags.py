# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" AFP slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper
#from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper, AllowedList

# create container
__author__  = 'L. Zivkovic'
__version__="$Revision: 1.0 $"
__doc__="AFP slice specific flags  "


_flags = []
class signatures(JobProperty):
    """ signatures in AFP slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []

_flags.append(signatures)


class AFPSlice(JobPropertyContainer, CommonSliceHelper):
    """ AFP Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(AFPSlice)

# add add common slice flags
TriggerFlags.AFPSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.AFPSlice.add_JobProperty(flag)
del _flags

# make an alias
AFPSliceFlags = TriggerFlags.AFPSlice

#  LocalWords:  allowedTypes
