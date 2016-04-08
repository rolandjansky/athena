# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" L1Calo slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'A. Cerri, after X. Wu example'
__version__="$Revision: 1.40 $"
__doc__="L1Calo slice specific flags  "


_flags = []

class signatures(JobProperty):
    """ List of slices provided by the L1Calo slice """ 
    statusOn=True
    allowedTypes=['list']
    allowedValues = AllowedList([
        'L1Calocalibration',
        'L1Calocalibration_L1BGRP7',
        ])
    StoredValue   = []                            
    
_flags.append(signatures)
    
# create container
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper

class L1CaloSlice(JobPropertyContainer, CommonSliceHelper):
    """ L1Calo Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(L1CaloSlice)

# add add common slice flags
TriggerFlags.L1CaloSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.L1CaloSlice.add_JobProperty(flag)
del _flags

# make an alias
L1CaloSliceFlags=TriggerFlags.L1CaloSlice
