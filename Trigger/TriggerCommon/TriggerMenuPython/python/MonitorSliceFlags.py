# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Flags for monitoring chains """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

_flags = [] 

class signatures(JobProperty):
    """ signatures in monitoring slice """
    statusOn=True
    allowedTypes=['list']
    allowedValues = AllowedList([
        'rate_monitor',
        'cost_monitor',
        'rate_monitor_rejAll',
        'cost_monitor_rejAll',
        'EFTRTOnly_FS_allL1',
        'EFInsideOut_FS_allL1',
        'L2IdScan_FS_allL1',
        'L2SiTrack_FS_allL1',
        'EFTRTOnly_FS_MB2',
        'EFInsideOut_FS_MB2',
        'L2IdScan_FS_MB2',
        'L2SiTrack_FS_MB2',
        'EFTRTOnly_FS',
        'EFInsideOut_FS',
        'L2IdScan_FS',
        'L2SiTrack_FS',
        'InDetMon_FS',
        'InDetMon_FS_R1',
        'InDetMonMergeDP_FS',
        'InDetMon_L2Star_FS',
        'InDetMon_HI_FS',
        'ROBRequestMon',
        'ROBRequestMonUnseeded',
        'HLTTimeBurner',
        'TRTMon_filled',
        'TRTMon_empty', 
        'IDMon_filled',
        'IDMon_empty',
        'IDMon_unpaired_iso',
        'TRTMon_filled',
        'TRTMon_empty',
        'TRTMon_unpaired_iso',
        ])
    StoredValue   = [
        'cost_monitor',
        'cost_monitor_rejAll',
        ]

_flags.append(signatures)

# create container
class MonitorSlice(JobPropertyContainer, CommonSliceHelper):
    """ Monitor Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(MonitorSlice)

## add add common slice flags
TriggerFlags.MonitorSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.MonitorSlice.add_JobProperty(flag)
del _flags

## make an alias
MonitorSliceFlags = TriggerFlags.MonitorSlice

