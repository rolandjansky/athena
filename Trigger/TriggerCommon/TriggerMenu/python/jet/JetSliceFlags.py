# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Jet slice specific flags  """

from AthenaCommon.JobProperties         import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

__author__  = 'T. Bold'
__version__="$Revision: 1.31 $"
__doc__="Jet slice specific flags  "


_flags = [] 
class signatures(JobProperty):
    """ signatures in Jet slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = []
    
_flags.append(signatures)



# create container

class JetSlice(JobPropertyContainer, CommonSliceHelper):
    """ Jet Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(JetSlice)

# add add common slice flags
TriggerFlags.JetSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.JetSlice.add_JobProperty(flag)
del _flags

# make an alias
JetSliceFlags = TriggerFlags.JetSlice

