# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Combined slices specific flags  """

from AthenaCommon.JobProperties         import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

__author__  = 'T. Bold, B. Demirkoz'
__version__="$Revision: 1.53 $"
__doc__="Flags for combined slices  "


_flags = [] 

class signatures(JobProperty):
    """ signatures in Combined slice """
    statusOn=True
    allowedTypes=['list']    
    StoredValue = []
    
_flags.append(signatures)

# create container
class CombinedSlice(JobPropertyContainer, CommonSliceHelper):
    """ Combined Slices Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(CombinedSlice)

for flag in _flags:
    TriggerFlags.CombinedSlice.add_JobProperty(flag)
del _flags

# make an alias
CombinedSliceFlags = TriggerFlags.CombinedSlice



