# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" EnhancedBias slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from TriggerMenu.menu.CommonSliceHelper import CommonSliceHelper

__author__  = 'P. Bell, after A. Cerri'
__version__="$Revision: 0.1 $"
__doc__="EnhancedBias slice specific flags  "


_flags = []

class SliceList(JobProperty):
    """ List of chains provided by the EnhancedBias slice """ 
    statusOn=True
    allowedTypes=['list']
    StoredValue=[]
_flags.append(SliceList)

class signatures(JobProperty):
    """ signatures in EnhancedBias slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue= []
_flags.append(signatures)

# create container

class EnhancedBiasSlice(JobPropertyContainer, CommonSliceHelper):
    """ EnhancedBias Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(EnhancedBiasSlice)

# add add common slice flags
TriggerFlags.EnhancedBiasSlice.import_JobProperties('TriggerMenu.menu.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.EnhancedBiasSlice.add_JobProperty(flag)
del _flags

# make an alias
EnhancedBiasSliceFlags = TriggerFlags.EnhancedBiasSlice

