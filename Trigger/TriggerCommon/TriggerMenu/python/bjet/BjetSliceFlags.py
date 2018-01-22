# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Bjet slice specific flags  """

from AthenaCommon.JobProperties         import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

__author__  = 'T. Bold'
__version__="$Revision: 1.31 $"
__doc__="Bjet slice specific flags  "


_flags = [] 
class signatures(JobProperty):
    """ signatures in Bjet slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []
    
_flags.append(signatures)



# create container

class BjetSlice(JobPropertyContainer, CommonSliceHelper):
    """ Bjet Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(BjetSlice)

# add add common slice flags
TriggerFlags.BjetSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.BjetSlice.add_JobProperty(flag)
del _flags

# make an alias
BjetSliceFlags = TriggerFlags.BjetSlice

