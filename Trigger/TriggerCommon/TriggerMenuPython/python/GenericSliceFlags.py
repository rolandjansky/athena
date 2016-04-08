# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Generic flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'T. Bold, I.Grabowska-Bold'
__version__="$Revision: 1.46 $"
__doc__="Generic flags  "


_flags = [] 

class signatures(JobProperty):
    """ signatures in Muon slice """
    statusOn=True
    allowedTypes=['list']
    allowedValues = AllowedList([
        'allL1_NoAlg',
        'L1RD0_firstempty_NoAlg',
        'L1BGRP7_NoAlg',
        'eb_physics',
        'high_eb_physics',
        'eb_physics_noL1PS',
        'eb_physics_L1PS',
        'eb_minbias',
        'eb_random',
        'eb_random_unpaired',
        'eb_physics_unpaired',
        'eb_random_unpaired_iso',
        'eb_random_unpaired_noniso',
        'eb_physics_unpaired_iso',
        'eb_physics_unpaired_noniso',
        'eb_physics_unpaired_iso_v1',
        'eb_physics_unpaired_noniso_v1',
        'eb_random_firstempty',
        'eb_physics_firstempty',
        'eb_random_empty',
        'eb_physics_empty',
        'eb_physics_empty_v1',
        ])
    StoredValue   = [
        'allL1_NoAlg', 'eb_physics', 'eb_minbias',
        ]

_flags.append(signatures)


# create container

class GenericSlice(JobPropertyContainer, CommonSliceHelper):
    """ Generic Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(GenericSlice)

## add add common slice flags
TriggerFlags.GenericSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.GenericSlice.add_JobProperty(flag)
del _flags

## make an alias
GenericSliceFlags = TriggerFlags.GenericSlice

