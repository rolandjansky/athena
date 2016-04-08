# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" MET slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'T. Bold'
__version__="$Revision: 1.25 $"
__doc__="MET slice specific flags  "


_flags = []
class signatures(JobProperty):
    """ signatures in MET slice """
    statusOn=True
    allowedTypes=['list']
    allowedValues = AllowedList( [
        'xe80_tclcw_L2L1Check',
        'xe10000_L1RD1',
        'te1000', 'te1100', 'te1200', 'te550', 'te700', 'te900',
        'te600', 'te1400', 'te1600', 
        'xe100_noMu', 'xe100_tight_noMu', 'xe110_noMu',
        'xe15_noL2',
        'xe20', 'xe20_noL2', 'xe20_noL2_noMu', 'xe20_noMu',
        'xe25_noL2_noMu', 'xe25_noMu',
        'xe30', 'xe30_noL2_noMu', 'xe30_noMu', 'xe30_noMu_L1EM14XE20', 'xe30_noMu_L1MU10XE20',
        'xe35_noL2_noMu', 'xe35_noMu', 'xe35_noMu_L1EM10XE20', 'xe35_noMu_L1MU10XE20',
        'xe40_loose_noMu', 'xe40_medium_noMu', 'xe40_noL2_noMu', 'xe40_noMu', 'xe40_tight_noMu','xe40_noL2',
        'xe45_loose_noMu', 'xe45_noL2_noMu','xe45_tight_noMu',
        'xe50_noMu',
        'xe55', 'xe55_loose_noMu', 'xe55_medium_noMu', 'xe55_noMu',
        'xe60_L2FEB_val', 'xe60_L2FEB_val_NoAlg', 'xe60_noMu', 'xe60_tight_L2FEB_val', 'xe60_tight_noMu', 'xe60_verytight_noMu',
        'xe45_LArNoiseBurst',
        'xe55_LArNoiseBurst',
        'xe60_LArNoiseBurst',
        #'xe65_loose',
        'xe65_noMu',
        'xe70_noMu',
        'xe70_noL2_noMu',
        'xe70_tight_noMu',
        'xe80_L2FEB_val', 'xe80_L2FEB_val_NoAlg',
        #'xe80_medium',
        'xe80_noMu', 'xe80_tight_noMu',
        'xe90_noMu', 'xe90_tight_noMu',
        'xeL2Mon_L1RD0FILLED', 'xeL2Mon_L1XE20', 'xe_NoCut_allL1', 'xe_NoCut_tclcw_allL1', 'xe_NoCut_allL1_noMu_Mon','xe_NoCut_FEB_allL1',
        #xs items
        'xs100_noMu', 'xs120_noMu',
        'xs30_noMu',
        'xs45_loose_noMu_L1XE10', 'xs45_medium_noMu', 'xs45_noMu', 'xs45_noMu_unseeded',
        'xs60_noMu', 'xs60_noMu_L1EM10XS45', 'xs60_noMu_unseeded',
        'xs70_noMu', 'xs70_tight_noMu',
        'xs75_noMu', 'xs75_noMu_L1EM10XS50',
        'xs80_loose_noMu', 'xs80_noMu',
        'xs90_noMu',
        ##v4
        'xe40','xe40_tight','xe45_tight', 
        'xe60_unseeded',
        'xe70_tight',
        'xe80',
        'xe90',
        'xe110',
        'xe30_noL2','xe40_noL2','xe50_noL2','xe60_noL2','xe70_noL2',
        #new V4 
        'xe50',
        'xe60',
        'xe70',
        'xe80_tight',
        'xe90_tight',
        'xe100','xe100_tight',
        ##XS
        'xs30', 
        'xs35_unseeded',
        'xs45',
        'xs60',
        'xs75', 
        'xs100',
        'xs120',
        'xs45_L1XS45',
        'xs45_L1XS50',
        'xs45_L1XS60',
        'xs45_L1XS55',
        'xs50_L1XS50',
        ##TopoCluster
        'xe30_tclcw',
        'xe70_tight_tclcw',
        'xe65_tclcw',
        'xe70_tclcw',
        'xe75_tight1_tclcw',
        'xe50_tclcw_noL2',
        'xe50_tclcw_unseeded',
        'xe50_tclcw_tight' ,
        'xe60_tclcw_noL2',
        #'xe60_loose_tclcw',
        'xe100_loose_tclcw',
        'xe60_tclcw_unseeded',
        'xe70_tclcw_unseeded',
        'xe80_tclcw_unseeded',
        'xe90_tclcw_unseeded',
        'xe60_tclcw_tight_unseeded', 
        'xe30_tcem_noL2',
        'xe35_tcem_noL2',
        ##New V4
        'xe55_tclcw',
        'xe55_tclcw_tight',
        'xe60_tclcw_loose','xe60_tclcw',
        'xe70_tclcw_loose','xe70_tclcw_veryloose',
        'xe75_tclcw',
        'xe80_tclcw_loose','xe80_tclcw','xe80_tclcw_tight','xe80_tclcw_verytight',
        'xe90_tclcw_veryloose','xe90_tclcw','xe90_tclcw_tight','xe90_tclcw_verytight','xe90_tclcw_loose',
        'xe100_tclcw_veryloose','xe100_tclcw','xe100_tclcw_loose','xe100_tclcw_verytight',
        'xe80_tclcw_veryloose', 'xe80T_tclcw_veryloose',
        'xe80_tclcw_veryloose_v2', 'xe80T_tclcw_veryloose_v2',
        ##XS
        #'xs100_tclcw',
        #'xs120_tclcw',
        'xe60T',
        'xe80T_loose', 'xe80T',
        'xe80T_tclcw_loose' , 'xe80T_tclcw'       ,
        'xe100T_tclcw_loose', 'xe100T_tclcw'      ,
        'xe60_tclcw_noL2_wMu',
        'xe40_tclcw_noL2_wMu',
        # For cross-checks
        'xe60T_tclcw',
        'xe60_tclcw_loose_v2',
        'xe60T_tclcw_loose',
        'xe60T_tclcw_loose_delayed','xe60_tclcw_loose_delayed',
        'xe70_tclcw_veryloose_v2',
        'xe70T_tclcw_veryloose',

        ])
    StoredValue   = [
        "xe20", "xe30", "xe55",
        #"xe80_medium",
        #"xe65_loose",
        "xe20_noMu", "xe25_noMu", "xe30_noMu", "xe40_noMu",
        "xe35_noMu",
        "xe40_medium_noMu", "xe40_loose_noMu", "xe45_loose_noMu",
        "xe55_noMu", 
        ]

_flags.append(signatures)



# create container
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper

class METSlice(JobPropertyContainer, CommonSliceHelper):
    """ MET Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(METSlice)

# add add common slice flags
TriggerFlags.METSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.METSlice.add_JobProperty(flag)
del _flags

# make an alias
METSliceFlags = TriggerFlags.METSlice

#  LocalWords:  allowedTypes
