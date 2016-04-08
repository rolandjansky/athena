# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Lvl1 specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList
from AthenaCommon.Logging import logging

__author__  = 'T. Bold'
__version__="$Revision: 1.78 $"
__doc__="Lvl1 specific flags"

log = logging.getLogger('TriggerMenuPython.Lvl1Flags.py')

_flags = [] 
class items(JobProperty):
    """ Items enabled in Lvl1 """
    statusOn=True
    allowedTypes=['list']
    allowedValues = AllowedList([
        # EM
        'L1_EM2','L1_EM3', 'L1_EM4', 'L1_EM7', 'L1_EM7I', 'L1_EM10VH', 'L1_EM13', 'L1_EM13I', 'L1_EM18', 'L1_EM10H', 'L1_EM16H', 'L1_EM15H', 'L1_EM18H',
        'L1_EM8', 'L1_EM6', 'L1_EM11H', 'L1_EM20H', 'L1_EM31', 'L1_2EM6', 'L1_2EM8', 'L1_2EM13', 'L1_2EM11H', 'L1_3EM6', 'L1_EM3_EM8', 'L1_2EM3_EM13',
        'L1_2EM5_EM13', 'L1_3EM6_EM11H',
        'L1_EM5', 'L1_EM5I', 'L1_EM10', 'L1_EM10I', 'L1_EM14', 'L1_EM14I', 'L1_EM16VH', 'L1_EM19I', 'L1_EM85', 'L1_EM16V', 'L1_EM14VH',
        'L1_EM18I', 'L1_EM18VH', 'L1_EM23', 'L1_EM23I', 'L1_EM50', 'L1_EM100', 
        'L1_2EM2','L1_2EM3', 'L1_2EM4', 'L1_2EM5', 'L1_2EM5I', 'L1_2EM7', 'L1_2EM7I', 'L1_2EM10', 'L1_2EM10I', 'L1_2EM13', 'L1_2EM13I', 'L1_2EM18', 'L1_2EM10VH', 'L1_2EM12',
        'L1_2EM14', 'L1_2EM14I', 'L1_2EM19I', 'L1_2EM3_EM5', 'L1_3EM10', 'L1_2EM10H', 'L1_2EM3_EM6', 'L1_2EM6_EM16VH', 'L1_3EM6_EM10VH', 'L1_2EM12_EM16V', 'L1_3EM6_EM14VH',
        'L1_2EM3_EM10', 'L1_2EM3_EM12', 'L1_2EM5_EM10', 'L1_2EM5_EM16H','L1_2EM5_EM10VH', 'L1_2EM5_EM16VH', 'L1_3EM5_EM10H', 'L1_3EM5_EM10VH', 'L1_3EM5_EM10', 'L1_2EM5_EM12', 
        'L1_2EM18I', 'L1_2EM23I', 'L1_2EM3_EM7', 'L1_3EM13', 'L1_3EM7', 'L1_3EM5',
        'L1_EM3_EMPTY', 'L1_EM4_EMPTY', 'L1_EM7_EMPTY',
        'L1_EM7I_EMPTY', 'L1_EM5_EMPTY', 'L1_EM6_EMPTY', 'L1_EM5I_EMPTY', 'L1_EM10_EMPTY', 'L1_EM13_EMPTY',
        'L1_EM13I_EMPTY', 'L1_EM18_EMPTY', 'L1_2EM3_EMPTY',
        'L1_2EM4_EMPTY', 'L1_2EM7_EMPTY', 'L1_2EM7I_EMPTY',
        'L1_2EM10_EMPTY', 'L1_2EM7_EM10', 'L1_2EM7_EM10VH',
        'L1_EM3_BGRP1', 'L1_EM3_BGRP3', 'L1_EM3_BGRP1_BPTX',
        'L1_EM10I_EMPTY', 'L1_EM14_EMPTY', 'L1_2EM5_EMPTY', 'L1_2EM5I_EMPTY',
        'L1_EM10V', 'L1_EM10I1', 'L1_EM10VI1', 
        # EM L1 scale
        'L1_2EM5', 'L1_2EM10I',
        'L1_EM12_3J10', 'L1_EM12_4J10',
        # MUON
        'L1_MU0', 'L1_MU4', 'L1_MU6', 'L1_MU10', 'L1_MU11', 'L1_MU15', 'L1_MU20', 'L1_MU40', 'L1_MU0_COMM',
        'L1_2MU0', 'L1_2MU4', 'L1_2MU6', 'L1_2MU10', 'L1_2MU11', 'L1_2MU15', 'L1_2MU15', 'L1_2MU20', 'L1_3MU0', 'L1_4MU0', 'L1_3MU4', 'L1_3MU6', 
        'L1_2MU0_MU6', 'L1_2MU4_MU6',
        'L1_MU0_EMPTY', 'L1_MU4_EMPTY', 'L1_MU6_EMPTY', 'L1_MU10_EMPTY', 'L1_MU15_EMPTY',
        'L1_MU20_EMPTY', 'L1_2MU0_EMPTY', 'L1_2MU4_EMPTY', 'L1_MU20_FIRSTEMPTY',
        'L1_MU0_TGC_HALO', 'L1_MU0_LOW_RPC', 'L1_MU6_RPC', 'L1_MU0_TGC',
        'L1_MU6_TGC', 'L1_MU0_HIGH_RPC',
        'L1_MU0_TGC_HALO_BPTX', 'L1_MU0_LOW_RPC_BPTX',
        'L1_MU6_RPC_BPTX', 'L1_MU0_TGC_BPTX', 'L1_MU6_TGC_BPTX', 'L1_MU0_HIGH_RPC_BPTX',
        'L1_MU0_TGC_HALO_EMPTY', 'L1_MU0_LOW_RPC_EMPTY', 'L1_MU0_TGC_EMPTY',
        'L1_MU0_HIGH_RPC_EMPTY', 'L1_MU0_COMM_EMPTY', 'L1_2MU4_BARRELONLY',
        'L1_2MU4_MU6_BARREL','L1_2MU4_BARREL',
        'L1_2MU6_BARREL','L1_2MU6_BARRELONLY','L1_2MU6_MU10','L1_3MU4_MU6',
        
        # TAU
        'L1_TAU3', 'L1_TAU5', 'L1_TAU6', 'L1_TAU9', 'L1_TAU9I', 'L1_TAU11I',  'L1_TAU16', 'L1_TAU16I',
        'L1_TAU8V', 'L1_TAU8I', 'L1_TAU8VI', 'L1_TAU11V', 
        'L1_TAU25', 'L1_TAU25I', 'L1_TAU40',
        'L1_TAU6I', 'L1_TAU8', 'L1_TAU11', 'L1_TAU11I', 'L1_TAU20', 'L1_TAU30', 'L1_TAU30I', 'L1_TAU30_NOTEM12','L1_TAU50',
        'L1_2TAU5', 'L1_2TAU8', 'L1_2TAU9', 'L1_2TAU9I', 'L1_2TAU25', 'L1_2TAU11I',
        'L1_2TAU16I', 'L1_2TAU16', 'L1_2TAU20',
        'L1_2TAU6', 'L1_2TAU6I', 'L1_2TAU11', 
        'L1_2TAU8I_TAU11', 'L1_2TAU8I_TAU15', 'L1_2TAU11I_TAU15',
        'L1_TAU5_EMPTY', 'L1_TAU6_EMPTY', 'L1_TAU9_EMPTY',
        'L1_TAU9I_EMPTY', 'L1_TAU16_EMPTY', 'L1_TAU16I_EMPTY', 'L1_TAU25_EMPTY',
        'L1_2TAU9_EMPTY', 'L1_2TAU9I_EMPTY', 'L1_2TAU16_EMPTY',
        'L1_2TAU16I_EMPTY',
        'L1_TAU5_BGRP1', 'L1_TAU5_BGRP3', 'L1_TAU5_BGRP1_BPTX',
        'L1_TAU6I_EMPTY', 'L1_TAU8_EMPTY', 'L1_TAU11_EMPTY', 'L1_TAU11I_EMPTY', 'L1_TAU20_EMPTY', 'L1_TAU30_EMPTY', 'L1_2TAU6I_EMPTY', 'L1_2TAU5_EMPTY', 'L1_2TAU6_EMPTY',
        'L1_2TAU11_EMPTY', 'L1_2TAU11I_EMPTY', 

        ## VBF
        'L1_2TAU8_TAU15_FJ15','L1_2TAU8_TAU15_J15CF','L1_2TAU8_TAU11I_EM10VH_FJ15', 'L1_2TAU8_TAU11I_EM10VH_J15CF',
        'L1_EM14VH_FJ15',  'L1_EM14VH_J15CF', 'L1_MU10_FJ15', 'L1_MU10_J15CF' ,
        'L1_3J15_FJ15',  'L1_2J15_2FJ15',
       
        # JET
        'L1_J5', 'L1_J5_win6', 'L1_J5_win8', 'L1_J10', 'L1_J10_win6', 'L1_J18', 'L1_J23', 'L1_J35', 
        'L1_J42', 'L1_J70', 'L1_J120', 'L1_J200',
        'L1_J20', 'L1_J40', 'L1_J100', 'L1_J130', 'L1_J150', 
        'L1_2J5', 'L1_2J10', 'L1_2J10_win6', 'L1_2J18',
        'L1_2J40', 'L1_2J70', 'L1_2J100',
        'L1_2J30', 'L1_2J55', 'L1_2J95', 
        'L1_3J10', 'L1_4J10', 'L1_3J23', 'L1_4J18', 'L1_4J23',
        'L1_3J35', 'L1_4J35', 'L1_3J42', 'L1_4J35',
        'L1_3J70',
        'L1_4J18_3J23_2J35_J42', 'L1_5J5_4J18_3J23_J42',
        'L1_4J23_J42',
        'L1_5J10','L1_6J10',
        'L1_2J10_J30','L1_2J10_J55','L1_2J10_J50','L1_2J10_J75','L1_2J15_J50','L1_2J15_J75',
        'L1_2J15_J95','L1_3J15_J50',
        'L1_J18_MV', 'L1_J18_LV', 'L1_J18_NL', 'L1_J10_MV','L1_J20_MV',
        'L1_J20_MV', 'L1_J20_LV', 'L1_J20_NL',
        'L1_J15_MV', 'L1_J15_LV', 'L1_J15_NL',
	'L1_J5_TE90', 'L1_2J5_TE90',
        'L1_MU0_MV', 'L1_MU0_MV_VTE50',
        'L1_MU0_VTE20', 'L1_MU0_VTE50', 'L1_MU0_TE50',
        'L1_EM3_2J5',
        'L1_EM3_VTE20', 'L1_EM3_VTE50', 'L1_EM3_TE50', 'L1_EM3_MV_VTE50',
        'L1_MU4_MV_VTE50', 'L1_MU4_VTE50', 'L1_MU4_TE50',
        'L1_2MU0_MV',
        'L1_EM2_MV', 'L1_EM3_MV', 'L1_TAU5_MV', 
        'L1_J5_EMPTY', 'L1_J10_EMPTY', 'L1_J10_win6_EMPTY',
        'L1_J30_EMPTY', 'L1_J20_EMPTY', 'L1_J40_EMPTY',
        'L1_J100_EMPTY', 'L1_J130_EMPTY', 'L1_J70_EMPTY',
        'L1_J5_BGRP1', 'L1_J5_BGRP3', 'L1_J5_BGRP1_BPTX',
        'L1_J15', 'L1_J15CF',
        'L1_J30', 'L1_J55', 'L1_J75', 'L1_2J15',
        'L1_J15_EMPTY', 'L1_J30_EMPTY', 'L1_J55_EMPTY', 'L1_J75_EMPTY', 'L1_2J5_EMPTY', 'L1_2J10_EMPTY', 'L1_2J10_win6_EMPTY', 'L1_2J15_EMPTY', 'L1_3J10_EMPTY',
        # new jet thresholds
        'L1_2J20', 'L1_3J5', 'L1_3J20', 'L1_3J40', 'L1_3J70','L1_2J75',
        'L1_3J10_J40', 'L1_3J20_J40',
        'L1_4J5', 'L1_4J20', 'L1_4J40',
        'L1_4J5_J15', 'L1_4J5_J30','L1_3J5_J30',
        'L1_4J10_3J20',
        'L1_3J10_2J30',
        'L1_MU0_J20', 'L1_MU0_J40','L1_MU0_J50', 'L1_MU0_J70', 'L1_MU0_J75', 'L1_MU0_J100',
        'L1_MU4_J10', 'L1_MU4_J15', 'L1_MU4_J30', 'L1_MU4_J50', 'L1_MU4_J75',  
        'L1_MU6_J15','L1_MU6_2J20',
        'L1_MU10_J10', 'L1_MU10_J20',
        'L1_4J10_MU15', 'L1_4J10_EM10', 'L1_4J10_EM10I', 'L1_4J10_EM13I', 
        'L1_J40_XE20', 'L1_2J20_XE20', 'L1_J20_XE15_EM10I', 'L1_J20_XE20_EM13I', 'L1_J20_XE20_MU15',
        'L1_TAU9I_2J5_J20', 'L1_TAU9I_2J5_J40',
        'L1_TAU9I_5J5_4J10', 'L1_TAU9I_5J5_4J20',
        'L1_TAU9I_3J5_2J40', 'L1_TAU9I_3J5_2J10', 'L1_TAU9I_3J5_2J20',     
        'L1_TAU6_3J5_2J20_XE25', 
        'L1_TAU9I_2J5_J70', 'L1_TAU9I_4J5_3J20',
        'L1_6J10_JE220', 'L1_4J20_JE220', 'L1_3J40_JE220',
        'L1_4J15_JE220', 'L1_3J30_JE220',
        'L1_MU0_J20_XE20', 'L1_MU4_J20_XE20', 'L1_MU4_J20_XE35',
        #JET L1 scale
        'L1_J15', 'L1_J30', 'L1_J55', 'L1_J75', 'L1_J95', 'L1_J115', 'L1_J350',
        'L1_3J15', 'L1_4J15', 'L1_5J15', 'L1_3J30', 'L1_4J30', 
        #EM L1 scale
        'L1_4J10_EM10I', 'L1_J40_XE15', 'L1_2J20_XE15',
        'L1_TAU5_4J5_3J15', 'L1_TAU6_4J5_3J15', 'L1_TAU6_5J5_4J15',
        'L1_TAU6_2J5_J30', 'L1_TAU6_2J5_J55',
        'L1_TAU5_3J5_2J15', 'L1_TAU6_3J5_2J15', 'L1_2TAU5_TAU6_EM5',
        'L1_TAU11I_XE15', 'L1_TAU6I_XE30',
        'L1_J55_XE15', 'L1_J15_XE15_MU15',
        'L1_TAU6I_5J5_4J10', 'L1_TAU6I_3J5_2J10',       
        # FJET 
        'L1_FJ3', 'L1_FJ10', 'L1_FJ18', 'L1_FJ100',
        'L1_FJ35', 'L1_FJ70', 'L1_FJ120',
        #pPb items
        'L1_FJ0','L1_FJ1','L1_FJ2','L1_FJ3',
        'L1_FJ0_A','L1_FJ1_A','L1_FJ2_A','L1_FJ3_A',
        'L1_FJ0_C','L1_FJ1_C','L1_FJ2_C','L1_FJ3_C',
        #
        'L1_FJ5','L1_FJ10', 'L1_FJ15', 'L1_FJ30','L1_FJ55','L1_FJ95',
        'L1_2FJ3', 'L1_2FJ5', 'L1_2FJ18',
        'L1_2FJ35', 'L1_2FJ70', 'L1_2FJ15',
        'L1_2FJ10', 'L1_2FJ30', 'L1_2FJ55',
        'L1_FJ0_EMPTY', 'L1_FJ1_EMPTY', 
        'L1_FJ3_EMPTY', 'L1_FJ18_EMPTY', 'L1_FJ5_EMPTY','L1_FJ10_EMPTY', 'L1_FJ30_EMPTY',
        'L1_FJ3_BGRP1', 'L1_FJ3_BGRP3', 'L1_FJ3_BGRP1_BPTX',
        #J-FJ
        'L1_J10_FJ10',
        'L1_J75_FJ75',
        #zero bias jet items
        'L1_ZB_J10',
        'L1_ZB_2J10',
        'L1_ZB',
        #lumi measurements
        'L1_MLZ_A', 'L1_MLZ_C', 'L1_MBLZ', 
        # HI jets
        'L1_J10_VTE500', 'L1_J10_VTE1000', 'L1_J10_VTE2000',
        'L1_J15_VTE500', 'L1_J15_VTE1000', 'L1_J15_VTE2000',
        'L1_J30_VTE1000', 'L1_J30_VTE2000', 'L1_J30_VTE4000',
        'L1_J55_VTE1000', 'L1_J55_VTE2000', 'L1_J55_VTE4000', 'L1_J55_VTE6000',        
        # Items for background and empty studies
        'L1_CALO_EMPTY', 'L1_CALO_UNPAIRED1', 'L1_CALO_UNPAIRED2',
        'L1_FJ0_UNPAIRED_ISO', 'L1_FJ1_UNPAIRED_ISO', 'L1_FJ5_UNPAIRED_ISO',
        'L1_J5_UNPAIRED', 'L1_FJ3_UNPAIRED', 'L1_FJ5_UNPAIRED','L1_FJ10_UNPAIRED', 'L1_FJ30_UNPAIRED',
        'L1_J5_FIRSTEMPTY', 'L1_FJ5_FIRSTEMPTY', 'L1_FJ10_FIRSTEMPTY', 'L1_FJ30_FIRSTEMPTY',
        'L1_TAU5_UNPAIRED', 'L1_EM2_UNPAIRED','L1_EM2_EMPTY','L1_EM2_FIRSTEMPTY','L1_EM3_UNPAIRED',
        'L1_TAU5_FIRSTEMPTY', 'L1_TAU15_FIRSTEMPTY','L1_TAU15_UNPAIRED_ISO','L1_TAU15_UNPAIRED_NONISO', 'L1_EM3_FIRSTEMPTY',
        'L1_MU0_UNPAIRED', 'L1_MU6_UNPAIRED', 'L1_MU15_UNPAIRED', 'L1_MU0_COMM_UNPAIRED','L1_MU10_UNPAIRED_ISO',
        'L1_MU0_FIRSTEMPTY', 'L1_MU4_FIRSTEMPTY', 'L1_MU6_FIRSTEMPTY', 'L1_MU10_FIRSTEMPTY','L1_2MU0_FIRSTEMPTY', 'L1_2MU4_FIRSTEMPTY', 'L1_MU0_COMM_FIRSTEMPTY',
        'L1_J10_UNPAIRED', 'L1_J30_UNPAIRED',
        'L1_J10_FIRSTEMPTY', 'L1_J30_FIRSTEMPTY',
        # XE
        'L1_XE10','L1_XE15', 'L1_XE20', 'L1_XE25', 'L1_XE30',
        'L1_XE35', 'L1_XE40', 'L1_XE50', 'L1_XE35_BGRP7', 'L1_XE40_BGRP7', 'L1_XE50_BGRP7',
        'L1_XE60', 'L1_XE70', 'L1_XE80', 'L1_XE90', 'L1_XE100', 'L1_XE110',
        'L1_XE120','L1_XE125',
        'L1_XE15_EMPTY', 'L1_XE25_EMPTY', 'L1_XE35_EMPTY', 'L1_XE50_EMPTY',
        'L1_JE120_EMPTY', 'L1_JE280_EMPTY',
        'L1_XE10_EMPTY', 'L1_XE20_EMPTY', 'L1_XE30_EMPTY', 
        # TE
        #pPb items
        'L1_TE0','L1_TE1', 'L1_TE2', 'L1_TE3', 'L1_TE4', 
        'L1_TE5', 'L1_TE10', 'L1_TE20',  'L1_TE30', 'L1_TE35',
        'L1_TE40', 'L1_TE50',  'L1_TE65', 'L1_TE90', 'L1_TE100', 'L1_TE115', 'L1_TE150', 'L1_TE180',
        'L1_TE250', 'L1_TE240' , 'L1_TE360', 'L1_TE480', 'L1_TE530', 'L1_TE650',
        'L1_TE300', 'L1_TE400', 'L1_TE500', 'L1_TE600', 'L1_TE700',
        'L1_TE800', 'L1_TE900', 'L1_TE1000', 'L1_TE2000', 'L1_TE4000',
        'L1_TE10_EMPTY', 'L1_TE30_EMPTY', 'L1_TE100_EMPTY',
        'L1_TE150_EMPTY', 'L1_TE180_EMPTY', 'L1_TE360_EMPTY',
        # JE
        'L1_JE60','L1_JE100','L1_JE120', 'L1_JE200', 'L1_JE220', 'L1_JE280', 'L1_JE300', 'L1_JE340','L1_JE400', 'L1_JE480','L1_JE140',
        'L1_JE500', 'L1_JE350',
        # EM + X
        'L1_EM10_XE15', 'L1_EM14_XE10', 'L1_EM14_XE15', 'L1_EM14I_XE20',         
        'L1_EM13_XE15', 'L1_EM18_XE15', 'L1_EM18_XE20', 'L1_EM18I_XE20', 
        'L1_2EM5_MU6', 'L1_2EM7_MU6', 'L1_2EM13_MU6', 'L1_2EM18_MU6', 
        'L1_EM5_MU6', 'L1_EM7_MU6', 'L1_EM10VH_MU6', 'L1_EM16VH_MU4', 'L1_EM10H_MU6', 'L1_EM16H_MU4',
        'L1_EM7_XE20', 'L1_2EM3_TAU6',
        'L1_EM13_MU15', 'L1_EM13I_MU15', 'L1_EM7I_MU6',
        'L1_EM18I_XE15', 'L1_EM13I_MU10','L1_EM13I_MU10I',
        'L1_EM10VH_XE20', 'L1_EM10VH_XE30', 'L1_EM10VH_XE35','L1_EM10H_XE20', 'L1_EM10H_XE30', 'L1_EM10H_XE35', 'L1_EM12_XE20', 'L1_EM12_XE30', 'L1_EM16V_XE20', 'L1_EM16V_XS45', 
        'L1_EM10VH_XE20_2J10_J30', 'L1_EM10VH_XE25_2J10_J30',
        'L1_EM7_XS30', 'L1_EM7_XS45', 'L1_EM12_XS30', 'L1_EM12_XS45', 'L1_EM12_XS50', 'L1_EM6_XS30', 'L1_EM6_XS45', 'L1_EM6_XS50', 'L1_EM6_XS60','L1_EM6_XS55',
        'L1_EM8_XS30', 'L1_EM8_XS45', 'L1_EM13_XS30', 'L1_EM13_XS45',
        'L1_EM18H_MU4', 'L1_EM6_2MU6', 'L1_EM11H_MU6',
        'L1_EM11H_XE20', 'L1_EM11H_XE30', 'L1_EM11H_XE35',
        'L1_EM6_MU10', 'L1_2EM6_MU6', 'L1_EM13_3J10',  'L1_EM13_4J10', 'L1_EM13_XE20', 'L1_EM13_XE30',
        # EM L1 scale
        'L1_EM5_XE15', 'L1_2EM5_MU6', 'L1_EM5_MU6', 'L1_EM5_MU10','L1_EM10_XE10',
        # TAU + EM
        'L1_2TAU5_EM5','L1_2TAU6_EM5', 'L1_2TAU6_EM7',
        'L1_2TAU6_EM10', 'L1_2TAU8_EM10','L1_2TAU8_EM10VH',
        'L1_2TAU8I_EM10VH', 'L1_2TAU8V_EM10VH', 'L1_2TAU6_TAU9I_EM7',
        'L1_2TAU9I_EM13I', 'L1_TAU9I_EM13I', 'L1_2TAU9I_EM18I',
        'L1_2TAU16I_EM13I',
        'L1_TAU25I_XE40', 
        'L1_TAU16_EM7',
        'L1_2TAU11_EM10', 'L1_2TAU11_EM10VH','L1_TAU11_XE10_3J10','L1_TAU11_XS35',  'L1_2TAU11I_EM14VH', 'L1_2TAU11_TAU20_EM14VH', 'L1_2TAU11_TAU20_EM10VH',
        'L1_TAU15','L1_TAU15I','L1_TAU15_XE20','L1_TAU15_XE35', 'L1_TAU15_XE40','L1_TAU15I_XE35', 'L1_TAU15I_XE40','L1_TAU15_XE25_3J15', 'L1_TAU20_XE40', 'L1_TAU20_XE35',
        'L1_TAU15_XE10_3J10', 'L1_TAU15_XE25_3J10', 'L1_TAU15_XE25_3J10_J30',
        'L1_2TAU5_EM7', 'L1_TAU5_MU6','L1_TAU6I_MU10','L1_TAU5_XE10', 'L1_TAU11_EM5', 'L1_TAU30_EM10VH', 'L1_TAU20_EM10VH', 'L1_TAU20_EM10H', 'L1_2TAU8_EM10H',  'L1_TAU20_EM11H', 'L1_2TAU8_EM11H', 'L1_2TAU11_EM11H',
        'L1_2TAU11_EM10H','L1_2EM3_EM8', 'L1_2EM6_EM18H', 'L1_EM6_MU6',
        'L1_TAU11_XE10','L1_TAU6_XE10', 'L1_TAU8_XE10',
        'L1_TAU6I_2J5_J40',
        'L1_TAU6I_5J5_4J20', 'L1_TAU6I_4J5_3J20','L1_TAU6I_3J5_2J20',
        'L1_2TAU5_TAU6I_EM5', 'L1_2TAU6I_EM10I', 'L1_TAU6I_MU6',
        'L1_TAU6I_XE20', 'L1_TAU11_XE20', 'L1_TAU6I_2J5_J20',
        'L1_TAU6I_2J5_J70','L1_TAU6I_MU15','L1_TAU6I_XE15', 'L1_TAU6I_XE10',
        # MU + X
        'L1_MU10_XE20', 'L1_MU10_XE25', 'L1_MU15_XE15', 'L1_MU20_XE15', 'L1_MU20_XE20','L1_MU40_XE15',
        'L1_2MU0_XE20', 'L1_2MU0_XE25', 'L1_2MU4_XE40',
        'L1_MU0_J10', 'L1_MU0_J5', 'L1_MU0_EM3', 'L1_EM3_MU0', 'L1_EM3_MU4', 'L1_MU6_J5', 'L1_MU10_J5', 
        'L1_MU0_J18', 'L1_MU0_J23', 'L1_MU0_J35', 'L1_MU0_J42',
        'L1_MU6_EM3', 'L1_2MU6_EM5', 'L1_EM5_2MU6', 'L1_2MU6_EM7', 'L1_MU10_J10', 'L1_MU0_EM3_EMPTY', 'L1_MU6_J5_EMPTY',
        'L1_MU6_EM3_EMPTY', 'L1_MU0_J5_EMPTY', 'L1_2MU4_XE30', 'L1_2MU4_EM3', 'L1_2MU4_2EM3',
        #EM L1 scale
        'L1_2MU6_EM5', 'L1_MU15_XE10',
        'L1_MU0_J15', 'L1_MU0_J30', 'L1_MU0_J55',
        'L1_TAU6_MU6', 'L1_TAU6_MU10', 'L1_TAU8I_MU10','L1_TAU6_MU15', 'L1_TAU8_MU10', 
        # TAU + MU
        'L1_TAU9I_MU6', 'L1_TAU9I_MU15', 'L1_TAU9I_MU10', 'L1_TAU16_MU10',
        # JET + X
        'L1_J70_XE20','L1_J70_XE80', 'L1_J20_XE20_EM13I', 'L1_J20_XE20_MU15',
        'L1_4J23_EM13I', 'L1_2J10_EM13I', 'L1_J50_XE35', 'L1_J50_XE40', 'L1_2J30_XE20',
        # EM L1 scale
        'L1_J30_XE10', 'L1_J30_XE15','L1_J50_XE15', 'L1_J30_XE25','L1_J30_XE40', 'L1_TAU11_MU10','L1_J30_XE50',
        'L1_2J15_XE10', 'L1_2J15_XE15', 'L1_2J15_XE25', 
        'L1_J15_XE15_EM10', 'L1_J15_XE15_EM10I', 'L1_J15_XE20_MU15', 
        'L1_3J10_J30', 'L1_3J15_J30', 'L1_4J10_3J15',
        'L1_3J10_2J20',
        'L1_4J5_J15','L1_4J5_J30','L1_3J5_J30',
        # TAU + XE
        'L1_TAU6_XE15',
        'L1_TAU6_XE20',
        'L1_TAU9_XE15',
        'L1_TAU16_XE15',
        'L1_TAU9I_XE15',
        'L1_TAU9I_XE15_EMPTY',
        'L1_TAU6I_XE10_EMPTY', 'L1_TAU6_XE10_EMPTY',
        'L1_TAU9I_XE20', 'L1_TAU9I_XE40',
        'L1_TAU9I_XE30', 
        'L1_TAU25_XE30', 'L1_TAU16I_XE20', 'L1_TAU9I_XE50', 'L1_TAU40_XE40',
        # TAU + JET
        'L1_TAU9I_2J5_J40','L1_TAU9I_2J5_J10',
        'L1_TAU9I_3J5_2J20',
        'L1_TAU13I_4J20',
        'L1_TAU16I_4J20', 
        # EM L1 scale
        'L1_TAU6I_2J5_J30', 'L1_TAU6I_5J5_4J15', 'L1_TAU6I_4J5_3J15', 'L1_TAU6I_3J5_2J15',
        'L1_TAU6I_2J5_J15', 'L1_TAU6I_2J5_J55',
       # Random
        'L1_RD0', 'L1_RD1', 'L1_RD0_FILLED', 'L1_RD0_EMPTY', 'L1_RD1_FILLED', 'L1_RD1_EMPTY',
        'L1_RD0_BGRP1', 'L1_RD1_NOISE', 'L1_RD1_BGRP4', 'L1_RD1_BGRP5', 'L1_RD1_HIST', 'L1_RD1_BGRP0',
        'L1_RD0_UNPAIRED1', 'L1_RD0_UNPAIRED2', 'L1_RD0_UNPAIRED', 'L1_RD0_FIRSTEMPTY',
        #
        'L1_LHCF',
        # Minimum bias
        'L1_MBTS_1', 'L1_MBTS_2', 'L1_MBTS_1_1', 'L1_MBTS_A', 'L1_MBTS_C',
        'L1_MBTS_2_2', 'L1_MBTS_2_2_EMPTY',
        'L1_MBTS_3_3', 'L1_MBTS_3_3_EMPTY',
        'L1_MBTS_4_4', 'L1_MBTS_4_4_EMPTY',
        'L1_MBTS_1_COMM', 'L1_MBTS_2_COMM', 'L1_MBTS_1_1_COMM',
        'L1_MBTS_1_BGRP1', 'L1_MBTS_2_BGRP7', 'L1_MBTS_2_BGRP1', 'L1_MBTS_1_1_BGRP1',
        'L1_MBTS_A_BPTX0', 'L1_MBTS_C_BPTX0', 'L1_MBTS_A_BPTX1', 'L1_MBTS_C_BPTX1',
        'L1_MBTS_1_EMPTY', 'L1_MBTS_1_1_EMPTY', 'L1_MBTS_2_EMPTY',
        'L1_MBTS_A_EMPTY', 'L1_MBTS_C_EMPTY',
        #for HI run
        'L1_MBTS_2_VTE50', 'L1_MBTS_2_VTE100',
        'L1_MBTS_2_VTE500', 'L1_MBTS_2_VTE1000',
        'L1_MBTS_1_1_VTE50', 'L1_MBTS_2_2_VTE50',
        'L1_ZDC_A', 'L1_ZDC_C', 'L1_ZDC_AND',
        'L1_ZDC_AND_UNPAIRED_ISO', 'L1_ZDC_AND_UNPAIRED_NONISO',
        'L1_ZDC_AND_VTE50', 'L1_ZDC_A_VTE20', 'L1_ZDC_C_VTE20', 'L1_ZDC_A_C_VTE50', 'L1_ZDC_A_C_TE50',
        'L1_ZDC_A_C', 'L1_ZDC', 'L1_ZDC_A_EMPTY', 'L1_ZDC_C_EMPTY', 'L1_ZDC_AND_EMPTY', 'L1_ZDC_EMPTY', 'L1_ZDC_A_C_EMPTY',
        'L1_ZDC_A_C_OVERLAY', 'L1_MBTS_1_1_OVERLAY',
        'L1_ZDC_UNPAIRED1', 'L1_ZDC_UNPAIRED2', 'L1_ZDC_UNPAIRED',
        # for pA run
        'L1_ZDC_MBTS_1', 'L1_ZDC_MBTS_2',
        'L1_ZDC_MBTS_1_1', 'L1_ZDC_MBTS_2_2',
        #
        'L1_TRTfast',
        'L1_MB_Col', 'L1_MB_BV','L1_MB_MV',
        'L1_BCM_Ideal', 'L1_BCM_Comb',
        'L1_BCM_AtoC', 'L1_BCM_CtoA', 'L1_BCM_Wide','L1_BCM_Wide_EMPTY','L1_BCM_Wide_UNPAIRED_ISO','L1_BCM_Wide_UNPAIRED_NONISO','L1_BCM_Wide_NoCol', 'L1_BCM_MUL_1',  'L1_BCM_MUL_2',  'L1_BCM_MUL_3',
        'L1_BCM_MUL_4',  'L1_BCM_MUL_5',  'L1_BCM_MUL_6',  'L1_BCM_MUL_7', 'L1_BCM_HT',
        'L1_BCM_AtoC_BPTX', 'L1_BCM_CtoA_BPTX', 'L1_BCM_Wide_BPTX', 'L1_BCM_MUL_1_BPTX',
        'L1_BCM_AtoC_EMPTY', 'L1_BCM_CtoA_EMPTY', 'L1_BCM_MUL_1_EMPTY',
        'L1_BCM_Wide_EMPTY', 'L1_BCM_MUL_2_EMPTY', 'L1_BCM_MUL_3_EMPTY',
        'L1_BCM_MUL_4_EMPTY', 'L1_BCM_MUL_5_EMPTY', 'L1_BCM_MUL_6_EMPTY',
        'L1_BCM_HT_EMPTY',
        'L1_BCM_HT_BGRP0','L1_BCM_Wide_BGRP0','L1_BCM_AC_CA_BGRP0',
        'L1_BCM_AC_CA_UNPAIRED_ISO', 'L1_BCM_AC_CA_UNPAIRED_NONISO',
        'L1_MBTSA0', 'L1_MBTSA1', 'L1_MBTSA2', 'L1_MBTSA3',
        'L1_MBTSA4', 'L1_MBTSA5', 'L1_MBTSA6', 'L1_MBTSA7',
        'L1_MBTSA8', 'L1_MBTSA9', 'L1_MBTSA10', 'L1_MBTSA11',
        'L1_MBTSA12', 'L1_MBTSA13', 'L1_MBTSA14', 'L1_MBTSA15',
        'L1_MBTSC0', 'L1_MBTSC1', 'L1_MBTSC2', 'L1_MBTSC3',
        'L1_MBTSC4', 'L1_MBTSC5', 'L1_MBTSC6', 'L1_MBTSC7',
        'L1_MBTSC8', 'L1_MBTSC9', 'L1_MBTSC10', 'L1_MBTSC11',
        'L1_MBTSC12', 'L1_MBTSC13', 'L1_MBTSC14', 'L1_MBTSC15',
        'L1_MBTS_1_Col', 'L1_MBTS_2_Col', 'L1_MBTS_1_1_Col',
        'L1_MBTS_1_UNPAIRED1', 'L1_MBTS_1_UNPAIRED2', 'L1_MBTS_1_UNPAIRED',
        'L1_MBTS_2_UNPAIRED1', 'L1_MBTS_2_UNPAIRED2', 'L1_MBTS_2_UNPAIRED',
        'L1_MBTS_1_1_UNPAIRED1', 'L1_MBTS_1_1_UNPAIRED2', 'L1_MBTS_1_1_UNPAIRED',
        'L1_MBTS_4_4_UNPAIRED1', 'L1_MBTS_4_4_UNPAIRED2', 'L1_MBTS_4_4_UNPAIRED',
        'L1_LUCID_A', 'L1_LUCID_C', 'L1_LUCID_A_C','L1_LUCID', 'L1_LUCID_COMM',
        'L1_LUCID_A_BPTX', 'L1_LUCID_C_BPTX', 'L1_LUCID_A_C_BPTX','L1_LUCID_BPTX',
        'L1_LUCID_A_FILLED', 'L1_LUCID_C_FILLED', 'L1_LUCID_A_C_FILLED','L1_LUCID_FILLED',
        'L1_LUCID_A_EMPTY', 'L1_LUCID_C_EMPTY', 'L1_LUCID_A_C_EMPTY',
        'L1_LUCID_EMPTY', 'L1_LUCID_COMM_EMPTY',
        'L1_LUCID_A_UNPAIRED1', 'L1_LUCID_A_UNPAIRED2', 'L1_LUCID_A_UNPAIRED', 
        'L1_LUCID_C_UNPAIRED1', 'L1_LUCID_C_UNPAIRED2', 'L1_LUCID_C_UNPAIRED',
        'L1_LUCID_A_C_UNPAIRED1', 'L1_LUCID_A_C_UNPAIRED2', 'L1_LUCID_A_C_UNPAIRED',
        'L1_LUCID_UNPAIRED_ISO', 'L1_LUCID_UNPAIRED_NONISO', 'L1_LUCID_COMM_UNPAIRED_ISO',
        'L1_MBTS_1_FIRSTEMPTY','L1_MBTS_2_FIRSTEMPTY','L1_MBTS_1_1_FIRSTEMPTY','L1_MBTS_4_4_FIRSTEMPTY',
        # Bunch groups & BPTX
        'L1_BGRP0', 'L1_BGRP1', 'L1_BGRP2', 'L1_BGRP3', 'L1_BGRP4', 'L1_BGRP5', 'L1_BGRP6', 'L1_BGRP7',
        'L1_BPTX0_0', 'L1_BPTX1_0','L1_BPTX0_0_1', 'L1_BPTX1_0_1',
        'L1_BPTX0_BGRP0', 'L1_BPTX1_BGRP0','L1_BPTX0_BGRP0_BGRP1', 'L1_BPTX1_BGRP0_BGRP1', 'L1_BPTX0_BPTX1_BGRP0',
        #Direct inputs
        'L1_Tile', 'L1_TRT', 'L1_L1A_Mon',
        'L1_NIM_S9C0A19', 'L1_NIM_S9C0A20', 'L1_NIM_S9C1C19', 'L1_NIM_S9C1C20', 'L1_NIM_S8C2B19', 'L1_NIM_S8C2B20', 'L1_NIM_S8C2B21', 'L1_NIM_S8C2B22',
        'L1_NIMDIR2', 'L1_NIMDIR3', 'L1_NIMDIR4', 'L1_NIMDIR5',
        'L1_NIMDIR6', 'L1_NIMDIR7', 'L1_NIMDIR8', 'L1_NIMDIR9', 'L1_NIMDIR10', 'L1_NIMDIR11',
        'L1_NIMDIR12', 'L1_NIMDIR13', 'L1_NIMDIR14', 'L1_NIMDIR15', 'L1_NIMDIR16', 'L1_NIMDIR17',
        'L1_NIMDIR18', 'L1_NIMDIR19', 'L1_NIMDIR20', 'L1_NIMDIR21', 'L1_NIMDIR22', 'L1_NIMDIR23',
        'L1_NIMDIR24', 'L1_NIMDIR25', 'L1_NIMDIR26', 'L1_NIMDIR27', 'L1_NIMDIR28', 'L1_NIMDIR29',
        'L1_NIMDIR30', 'L1_NIMDIR31', 'L1_NIMDIR32', 'L1_NIMDIR33', 'L1_NIMDIR34', 'L1_NIMDIR35',
        'L1_NIMDIR36', 'L1_NIM_S8C2B23', 'L1_NIM_S8C2B24', 'L1_NIM_S8C2B25', 'L1_NIM_S8C2B26',
        'L1_NIM_S8C2B27', 'L1_NIM_S9C0B21', 'L1_NIM_S9C0B22', 'L1_NIM_S9C0B23', 'L1_NIM_S9C0B24', 'L1_NIM_ALFA_LOG_OD',
        'L1_MUB', 'L1_MUE',
        # Calibration requests
        'L1_CALREQ0', 'L1_CALREQ1', 'L1_CALREQ2',
        # Cosmics
        'L1_1NIM0','L1_1NIM1','L1_1PCLK0','L1_1PCLK1',
        'L1_1RNDM0','L1_1RNDM1','L1_1MU1','L1_1MU3','L1_1J7','L1_1J8',
        'L1MU2',
        # Debug
        'L1_EM3_BPTX', 'L1_TAU5_BPTX', 'L1_J5_BPTX', 'L1_FJ3_BPTX', 'L1_FJ18_BPTX',
        # Spares
        'L1_EM3_SPARE', 'L1_J5_SPARE', 'L1_TAU5_SPARE', 'L1_MU0_TGC_HALO_SPARE', 'L1_MU0_LOW_RPC_SPARE', 'L1_XE20_SPARE',
        'L1_TAU8_UNPAIRED_ISO', 'L1_TAU8_UNPAIRED_NONISO', 'L1_TAU8_FIRSTEMPTY',
        #UNPAIRED_ISO and NONISO
        'L1_EM2_UNPAIRED_ISO','L1_MU0_UNPAIRED_ISO', 'L1_MU4_UNPAIRED_ISO', 'L1_MU4_UNPAIRED_NONISO', 'L1_MU0_COMM_UNPAIRED_ISO','L1_TAU5_UNPAIRED_ISO','L1_J5_UNPAIRED_ISO','L1_J10_UNPAIRED_ISO','L1_J30_UNPAIRED_ISO',
        'L1_FJ10_UNPAIRED_ISO', 'L1_FJ10_UNPAIRED_NONISO', 'L1_FJ30_UNPAIRED_ISO',
        'L1_RD0_UNPAIRED_ISO', 'L1_RD0_UNPAIRED_NONISO',
        'L1_ZDC_UNPAIRED_ISO','L1_ZDC_UNPAIRED_NONISO','L1_MBTS_1_UNPAIRED_ISO','L1_MBTS_1_UNPAIRED_NONISO','L1_MBTS_2_UNPAIRED_ISO','L1_MBTS_1_1_UNPAIRED_ISO', 
        'L1_MBTS_2_2_UNPAIRED_ISO', 'L1_MBTS_3_3_UNPAIRED_ISO', 
        'L1_MBTS_4_4_UNPAIRED_ISO','L1_LUCID_A_UNPAIRED_ISO','L1_LUCID_C_UNPAIRED_ISO','L1_LUCID_A_C_UNPAIRED_ISO','L1_LUCID_A_C_UNPAIRED_NONISO','L1_MBTS_4_4_UNPAIRED_NONISO',
        'L1_MBTS_2_UNPAIRED_NONISO',
        'L1_EM2_UNPAIRED_NONISO','L1_MU0_UNPAIRED_NONISO','L1_MU0_COMM_UNPAIRED_NONISO','L1_TAU5_UNPAIRED_NONISO','L1_J5_UNPAIRED_NONISO','L1_J10_UNPAIRED_NONISO','L1_J30_UNPAIRED_NONISO','L1_TAU5_UNPAIRED_NONISO',
        ## New for v2
        'L1_J20','L1_J50','L1_J175','L1_J250',
        'L1_FJ50','L1_FJ75','L1_EM16','L1_EM35',
        'L1_TE300','L1_TE550', 'L1_EM3_UNPAIRED_ISO', 'L1_EM3_UNPAIRED_NONISO','L1_2FJ50','L1_J30_FJ30','L1_J50_FJ50',
        'L1_3J50', 'L1_3J75', 'L1_5J20',
        'L1_2J10_J30_JE140',
        'L1_TAU6_J50_XE20',
        'L1_2TAU5_EM10',
        'L1_2TAU11_TAU15',
        'L1_2TAU11_TAU20',
        'L1_2TAU15_TAU20',
        'L1_2TAU15',
        'L1_TAU11_XE15',
        'L1_2TAU6_EM10_J5',
        'L1_2TAU6_EM10_J10',
        'L1_2TAU6_MU10_J5',
        'L1_2TAU6_MU10_J10',
        'L1_3MU0',
        'L1_4MU0',
        'L1_3MU6',
        'L1_J50_XE20',
        'L1_J50_XE25',
        'L1_J50_XE30',
        'L1_2J20_XE20',
        'L1_2J15_XE30',
        'L1_3J20',
        'L1_3J10_J50',
        'L1_3J10_2J75',
        'L1_TAU6_J50_XE20',  
        'L1_2EM5_MU0',
        'L1_2MU0_EM5',
        'L1_EM5_MU0',
        'L1_EM10_MU0',
        'L1_EM10_MU6',
        'L1_EM10_XE15',
        'L1_3EM5',
        'L1_TAU11_XE20','L1_J15_XE20_EM10','L1_J15_XE20_MU15','L1_EM14_XE20','L1_EM10_XE20','L1_EM10_XE30',
        'L1_MU0_J15_UNPAIRED','L1_MU0_J15_EMPTY','L1_MU0_J15_FIRSTEMPTY',
        'L1_MU4_J15_UNPAIRED','L1_MU4_J15_EMPTY','L1_MU4_J15_FIRSTEMPTY',
        'L1_2MU6_UNPAIRED','L1_2MU6_EMPTY','L1_2MU6_FIRSTEMPTY',
        'L1_XS25', 'L1_XS30', 'L1_XS40','L1_XS45', 'L1_XS55',
        'L1_MU11_EMPTY','L1_MU0_MU6',
        'L1_TE750', 'L1_XS15', 'L1_XS20', 'L1_XS35', 'L1_XS50', 'L1_XS60', 'L1_XS65',
        'L1_EM12','L1_EM30', 'L1_EM30_BGRP7', 
        'L1_EM10_NL','L1_MU6_NL','L1_2EM3_NL','L1_2MU0_NL',
        'L1_EM3_NL','L1_MU0_NL',
        'L1_2J15_NL','L1_2EM5_NL',
        'L1_2TAU8_TAU11','L1_EM3_MU6',
        'L1_2TAU11_J10', 'L1_2TAU6_TAU11_J5',
        'L1_2TAU6_TAU11_J10', 'L1_2TAU8_TAU11_J10',
        'L1_TAU11_XS25', 'L1_TAU11_XS30','L1_TAU11_XS35', 'L1_TAU11_XS40', 'L1_TAU11_XS15_3J10',
        'L1_TAU15_XS35', 'L1_TAU15_XS25_3J10',
        'L1_J50_XS15','L1_J50_XS25',
        'L1_EM10_XS45', 'L1_EM10_XS50',
        'L1_ALFA_EMPTY','L1_ALFA_BGRP7', 'L1_ALFA_EMPTY_OD','L1_ALFA_BGRP7_OD',
        'L1_MU0_J15_UNPAIRED_ISO','L1_MU0_J15_UNPAIRED_NONISO',
        'L1_MU4_J15_UNPAIRED_ISO','L1_MU4_J15_UNPAIRED_NONISO',
        'L1_2MU6_UNPAIRED_ISO','L1_2MU6_UNPAIRED_NONISO',
        'L1_J30_XE35',
        'L1_MBTS_1_BGRP7','L1_LUCID_BGRP7',
        'L1_ZDC_A_C_BGRP7', 'L1_ZDC_A_BGRP7', 'L1_ZDC_C_BGRP7',
        'L1_2MU6_NL',
        'L1_BCM_Wide_NoCol','L1_ZDC_A_C_NoCol','L1_LUCID_A_C_NoCol','L1_MBTS_4_4_BGRP0_noCALIB',
        'L1_ZDC_A_C_UNPAIRED_ISO', 'L1_ZDC_A_C_UNPAIRED_NONISO',
        'L1_TAU11_2J10_J20_XE20',
        #HI
        'L1_J15_NZ', 'L1_EM3_NZ', 'L1_MU0_NZ',
        'L1_2J15_NZ', 'L1_2EM3_NZ', 'L1_2MU0_NZ',
        'L1_TE5_NZ',
        # ALFA items
        'L1_ALFA_ELAST1','L1_ALFA_ELAST2','L1_ALFA_ELAST3','L1_ALFA_ELAST4','L1_ALFA_ELAST5', 'L1_ALFA_ELAST6','L1_ALFA_ELAST7','L1_ALFA_ELAST8','L1_ALFA_ELAST9',
        'L1_ALFA_ELAST10', 'L1_ALFA_ELAST11', 'L1_ALFA_ELAST12', 'L1_ALFA_ELAST13', 'L1_ALFA_ELAST14', 'L1_ALFA_ELAST15','L1_ALFA_ELAST16','L1_ALFA_ELAST17','L1_ALFA_ELAST18',
        'L1_ALFA_SDIFF1','L1_ALFA_SDIFF2','L1_ALFA_SDIFF3','L1_ALFA_SDIFF4','L1_ALFA_SDIFF5','L1_ALFA_SDIFF6','L1_ALFA_SDIFF7','L1_ALFA_SDIFF8',
        'L1_ALFA_SHOW1','L1_ALFA_SHOW2','L1_ALFA_SHOW3','L1_ALFA_SHOW4','L1_ALFA_SHOW5','L1_ALFA_SHOW6',
        'L1_ALFA_SYST1','L1_ALFA_SYST2','L1_ALFA_SYST3','L1_ALFA_SYST4',
        'L1_ALFA_SHOWSYST1','L1_ALFA_SHOWSYST2','L1_ALFA_SHOWSYST3','L1_ALFA_SHOWSYST4','L1_ALFA_SHOWSYST5',
        'L1_ALFA_SYST5','L1_ALFA_SYST6','L1_ALFA_SYST7','L1_ALFA_SYST8','L1_ALFA_SYST9','L1_ALFA_SYST10','L1_ALFA_SYST11','L1_ALFA_SYST12','L1_ALFA_SYST13','L1_ALFA_SYST14','L1_ALFA_SYST15',
        'L1_ALFA_SYST16','L1_ALFA_SYST17','L1_ALFA_SYST18','L1_ALFA_SYST19','L1_ALFA_SYST20',
        'L1_ALFA_ANY','L1_ALFA_ANY_EMPTY','L1_ALFA_ANY_FIRSTEMPTY','L1_ALFA_ANY_UNPAIRED_ISO','L1_ALFA_ANY_UNPAIRED_NONISO',
        #new ALFA items 2012
        'L1_ALFA_B7L1U_OD','L1_ALFA_B7L1L_OD','L1_ALFA_A7L1U_OD','L1_ALFA_A7L1L_OD',	  
        'L1_ALFA_A7R1U_OD','L1_ALFA_A7R1L_OD','L1_ALFA_B7R1U_OD','L1_ALFA_B7R1L_OD',	  
        'L1_ALFA_B7L1_OD','L1_ALFA_A7L1_OD','L1_ALFA_A7R1_OD','L1_ALFA_B7R1_OD',	  
        'L1_ALFA_B7L1U','L1_ALFA_B7L1L','L1_ALFA_A7L1U','L1_ALFA_A7L1L',		
        'L1_ALFA_A7R1U','L1_ALFA_A7R1L','L1_ALFA_B7R1U','L1_ALFA_B7R1L',		    
        'L1_ALFA_B1_EMPTY', 'L1_ALFA_B2_EMPTY',	
                
        'L1_BGRP1_ALFA_BGT','L1_BGRP4_ALFA_BGT','L1_BGRP7_ALFA_BGT',
        'L1_MBTS_2_ALFA_UNPAIRED_ISO',
        'L1_MBTS_2_C_ALFA_A','L1_MBTS_2_A_ALFA_C','L1_MBTS_2_ALFA',		
        'L1_MBTS_1_C_ALFA_A','L1_MBTS_1_A_ALFA_C',
        'L1_LUCID_A_ALFA_C','L1_LUCID_C_ALFA_A', 'L1_LUCID_ALFA',  
        'L1_ZDC_A_ALFA_C','L1_ZDC_C_ALFA_A','L1_ZDC_ALFA',

        'L1_LUCID_A_ALFA_C_UNPAIRED_ISO','L1_LUCID_C_ALFA_A_UNPAIRED_ISO',
        'L1_MBTS_2_C_ALFA_A_UNPAIRED_ISO','L1_MBTS_2_A_ALFA_C_UNPAIRED_ISO',
        'L1_MBTS_1_C_ALFA_A_UNPAIRED_ISO','L1_MBTS_1_A_ALFA_C_UNPAIRED_ISO',

        'L1_EM3_ALFA_ANY', 'L1_EM3_ALFA_MBTS_C', 'L1_EM3_ALFA_MBTS_A',
        'L1_EM3_ALFA_ANY_UNPAIRED_ISO', 'L1_EM3_ALFA_MBTS_C_UNPAIRED_ISO', 'L1_EM3_ALFA_MBTS_A_UNPAIRED_ISO',

        ])

    
    StoredValue   = [
        # EM
        'L1_EM3', 'L1_EM7', 'L1_EM13', 'L1_EM13I', 'L1_EM18',
        'L1_EM18I', 'L1_EM23I', 'L1_EM100', 
        'L1_2EM3', 'L1_2EM7', 'L1_2EM13', 'L1_2EM13I', 'L1_2EM18',
        'L1_2EM18I', 'L1_2EM23I', 'L1_2EM3_EM7', 'L1_3EM5', 'L1_3EM7',
        # MUON
        'L1_MU0', 'L1_MU6', 'L1_MU10', 'L1_MU11', 'L1_MU20', 'L1_MU40', 
        'L1_2MU0', 'L1_2MU6', 'L1_2MU10', 'L1_2MU15', 'L1_2MU20', 'L1_3MU0', 'L1_4MU0', 'L1_3MU4', 'L1_3MU6',
        'L1_2MU0_MU6',
        # TAU
        'L1_TAU5', 'L1_TAU6', 'L1_TAU9I', 'L1_TAU9I', 'L1_TAU16I',
        'L1_TAU25', 'L1_TAU25I', 'L1_TAU40',
        'L1_2TAU9I', 'L1_2TAU16I', 'L1_2TAU16', 
        # JET
        'L1_J5', 'L1_J10', 'L1_J18', 'L1_J23', 'L1_J35', 
        'L1_J42', 'L1_J70', 'L1_J120', 
        'L1_3J10', 'L1_4J10', 'L1_3J23', 'L1_4J18', 'L1_4J23',
        'L1_3J35', 'L1_4J35', 'L1_3J42','L1_3J70', 
        'L1_4J23_J42',
        'L1_4J18_3J23_2J35_J42', 'L1_5J5_4J18_3J23_J42',
        'L1_5J10','L1_6J10',
        # FJET
        'L1_FJ18', 'L1_FJ35', 'L1_FJ70', 'L1_FJ120',
        'L1_2FJ18', 'L1_2FJ35', 'L1_2FJ70', 
        # XE
        'L1_XE15', 'L1_XE20', 'L1_XE25', 'L1_XE30', 'L1_XE35', 'L1_XE40', 'L1_XE60',
        'L1_XE70', 'L1_XE80', 
        # TE
        'L1_TE5', 'L1_TE10',  'L1_TE30', 'L1_TE40', 'L1_TE100', 'L1_TE150',
        'L1_TE180', 'L1_TE250', 'L1_TE360', 'L1_TE650',
        # JE
        'L1_JE120', 'L1_JE220', 'L1_JE280', 'L1_JE340',
        # EM + X
        'L1_EM13_XE15', 'L1_EM18_XE15', 'L1_EM18_XE20', 
        'L1_2EM7_MU6', 'L1_2EM13_MU6', 'L1_2EM18_MU6', 
        'L1_EM7_MU6', 'L1_EM7_XE20', 'L1_2EM3_TAU6',
        'L1_EM13_MU15', 'L1_EM13I_MU15', 
        # TAU + EM
        'L1_2TAU6_EM7', 'L1_2TAU6_TAU9I_EM7',
        'L1_2TAU9I_EM13I', 'L1_TAU16_EM7',
        # MU + X
        'L1_MU15_XE15', 'L1_MU20_XE15',
        'L1_MU0_J10', 'L1_MU0_J5', 'L1_MU6_J5','L1_MU10_J5',
        'L1_MU0_J18', 'L1_MU0_J23', 'L1_MU0_J35', 'L1_MU0_J42', 
        'L1_MU6_EM3', 'L1_2MU6_EM7', 'L1_EM3_MU6', 
        # TAU + MU
        'L1_TAU9I_MU6', 'L1_TAU9I_MU10', 'L1_TAU16_MU10',
        # JET + X
        'L1_J70_XE20', 'L1_J70_XE80', 'L1_J20_XE20_EM13I', 'L1_J20_XE20_MU15',
        'L1_4J23_EM13I', 'L1_2J10_EM13I', 
        # TAU + XE
        'L1_TAU9_XE15',
        'L1_TAU9I_XE15',
        'L1_TAU9I_XE20', 'L1_TAU9I_XE40',
        'L1_TAU9I_XE30', 
        'L1_TAU25_XE30', 'L1_TAU16I_XE20', 'L1_TAU9I_XE50', 'L1_TAU40_XE40',
        # TAU + JET
        'L1_TAU9I_2J5_J40','L1_TAU9I_2J5_J10',
        'L1_TAU9I_5J5_4J20',
        'L1_TAU9I_3J5_2J20',      
        # Minimum bias
        'L1_RD0', 'L1_RD1', 'L1_RD0_FILLED', 'L1_RD1_NOISE', 'L1_RD1_BGRP4', 'L1_RD1_BGRP5', 'L1_RD1_HIST', 'L1_RD1_BGRP0',
        'L1_MBTS_1', 'L1_MBTS_2', 'L1_MBTS_1_1',
        'L1_BCM_AtoC', 'L1_BCM_CtoA', 'L1_BCM_Wide', 'L1_BCM_Ideal',
        'L1_LUCID_A', 'L1_LUCID_C', 'L1_LUCID_A_C','L1_LUCID',
        # Calibration requests
        'L1_CALREQ0', 'L1_CALREQ1', 'L1_CALREQ2',
        #UNPAIRED_ISO and NONISO
        'L1_EM2_UNPAIRED_ISO','L1_MU0_UNPAIRED_ISO','L1_MU0_COMM_UNPAIRED_ISO','L1_TAU5_UNPAIRED_ISO','L1_J5_UNPAIRED_ISO','L1_J10_UNPAIRED_ISO','L1_J30_UNPAIRED_ISO','L1_FJ10_UNPAIRED_ISO','L1_RD0_UNPAIRED_ISO','L1_ZDC_UNPAIRED_ISO','L1_MBTS_1_UNPAIRED_ISO','L1_MBTS_2_UNPAIRED_ISO','L1_MBTS_1_1_UNPAIRED_ISO','L1_MBTS_4_4_UNPAIRED_ISO','L1_LUCID_A_UNPAIRED_ISO','L1_LUCID_C_UNPAIRED_ISO','L1_LUCID_A_C_UNPAIRED_ISO',
        'L1_MBTS_2_UNPAIRED_NONISO',
        ]
    
class thresholds(JobProperty):
    """ Items enabled in Lvl1 """
    statusOn=True
    allowedTypes=['list']
    allowedValues = AllowedList([
        'NIM0','NIM1','PCLK0','PCLK1','RNDM0','RNDM1','MU1','MU3','J7','J8',
        'MU2',
        #
        'MU0', 'MU4', 'MU6', 'MU10', 'MU11', 'MU15', 'MU20', 'MU40','MU0_COMM',
        'MU0_TGC_HALO', 'MU0_LOW_RPC', 'MU6_RPC', 'MU0_TGC', 'MU6_TGC', 'MU0_HIGH_RPC',
        'EM2', 'EM3', 'EM4', 'EM5', 'EM5I','EM7', 'EM7I', 'EM10', 'EM13', 'EM13I', 'EM6', 'EM8', 'EM11H', "EM20H", 'EM31',
        'EM18', 'EM18I', 'EM23', 'EM23I', 'EM50', 'EM100', 'EM15H', 'EM10H', 'EM16H', 'EM18H', 'EM16V', 'EM14VH',
        'EM5', 'EM10I', 'EM14', 'EM14I', 'EM19I', 'EM85',
        'EM10V', 'EM10I1', 'EM10VI1', 'EM10VH', 'EM16VH', 'EM18VH', 
        'HA3', 'HA5', 'HA6', 'HA9', 'HA9I', 'HA11I', 'HA13I','HA15','HA15I','HA16', 'HA16I',
        'HA8V', 'HA8I', 'HA8VI', 'HA11V', 
        'HA25', 'HA25I', 'HA40',
        'HA6I', 'HA8', 'HA11', 'HA11I', 'HA20', 'HA30', 'HA30I', 'HA50',
        'J5', 'J5_win6', 'J5_win8', 'J10','J10_win6', 'J18', 'J23', 'J35', 'J42', 'J70', 'J120', 'J200',
        # new jet thresholds
        'J15', 'J30', 'J55', 'J75',
        'J20', 'J40', 'J100', 'J130', 'J150',
        'J15CF', 'J30', 'J55', 'J75', 'J95', 'J115', 'J350', 
        'JE60','JE100','JE120', 'JE200', 'JE220', 'JE280','JE300', 'JE340', 'JE400', 'JE500','JE140', 'JE350',
        'JB0', 'JB1', 'JB2',
        'JF0', 'JF1', 'JF2',
        'JB3', 'JB18', 'JB35', 'JB70', 'JB120',  'JB100',
        'JF3', 'JF18', 'JF35', 'JF70', 'JF120', 'JF100',
        'JB5', 'JB10', 'JB15', 'JB20', 'JB30', 'JB40', 'JB55', 'JB95',
        'JF5', 'JF10', 'JF15', 'JF20', 'JF30', 'JF40', 'JF55', 'JF95',
        #pPb thresholds
        'TE0', 'TE1', 'TE2', 'TE3', 'TE4',
        'TE5', 'TE10','TE20', 'TE30', 'TE35', 'TE40', 'TE50', 'TE65', 'TE90', 'TE100', 'TE115', 'TE150', 'TE180', 
        'TE250', 'TE360', 'TE480', 'TE650', 'TE530', 
        'TE300', 'TE400', 'TE500', 'TE600', 'TE700', 'TE800',
        'TE900', 'TE1000','TE1200','TE1400',
        'TE2000', 'TE3000', 'TE4000', 'TE6000',
        'XE10','XE15', 'XE20', 'XE25', 'XE30',
        'XE35', 'XE40', 'XE50', 'XE60', 'XE70', 'XE80',
        'XE90', 'XE100', 'XE110','XE120', 'XE125', 
        'MBTS_A', 'MBTS_C', 
        'BCM_AtoC', 'BCM_CtoA', 'BCM_Comb', 'BCM_Wide', 'BCM_A1', 'BCM_A2', 'BCM_C1', 'BCM_C2', 'BCM_HTA', 'BCM_HTC',
        'MBTS_A0', 'MBTS_A1', 'MBTS_A2', 'MBTS_A3', 'MBTS_A4', 'MBTS_A5', 'MBTS_A6',
        'MBTS_A7', 'MBTS_A8', 'MBTS_A9', 'MBTS_A10', 'MBTS_A11', 'MBTS_A12', 'MBTS_A13',
        'MBTS_A14', 'MBTS_A15', 'MBTS_C0', 'MBTS_C1', 'MBTS_C2', 'MBTS_C3', 'MBTS_C4',
        'MBTS_C5', 'MBTS_C6', 'MBTS_C7', 'MBTS_C8', 'MBTS_C9', 'MBTS_C10', 'MBTS_C11',
        'MBTS_C12', 'MBTS_C13', 'MBTS_C14', 'MBTS_C15', 'BPTX0', 'BPTX1',
        'LUCID_A','LUCID_C',
        'ZDC_A', 'ZDC_C','ZDC_AND','TRTfast', 
        'NIMDIR2', 'NIMDIR3', 'NIMDIR4', 'NIMDIR5',
        'NIMDIR6', 'NIMDIR7', 'NIMDIR8', 'NIMDIR9', 'NIMDIR10', 'NIMDIR11',
        'NIMDIR12', 'NIMDIR13', 'NIMDIR14', 'NIMDIR15', 'NIMDIR16', 'NIMDIR17',
        'NIMDIR18', 'NIMDIR19', 'NIMDIR20', 'NIMDIR21', 'NIMDIR22', 'NIMDIR23',
        'NIMDIR24', 'NIMDIR25', 'NIMDIR26', 'NIMDIR27', 'NIMDIR28', 'NIMDIR29',
        'NIMDIR30', 'NIMDIR31', 'NIMDIR32', 'NIMDIR33', 'NIMDIR34', 'NIMDIR35',
        'NIMDIR36',
        'NIM28','NIM29','NIM30','NIM_MUB', 'NIM_MUE',
        # Debug - these look like items? PJB 15.2.2010
        'L1_EM3_BPTX', 'L1_TAU5_BPTX', 'L1_J5_BPTX', 'L1_FJ18_BPTX',
        # ZB jet thresholds
        'ZB_J10',
        'ZB_2J10',
        'ZB_J50', 'ZB_J75',
        'ZB_4MBTS_A',
        'ZB_EM14','ZB_EM12',
        ## New for v2
        'J20','J50','J175','J250',
        'JF50','JB50',
        'JF75','JB75',
        'EM16','EM35',
        'XS25', 'XS30', 'XS40', 'XS45', 'XS65', 'XS55',
        'TE750', 'XS15', 'XS20', 'XS35', 'XS50', 'XS60','TE550', 'TE240',
        'EM12','EM30',
        #### Forced for ALFA
        'NIM_B7R1L','NIM_B7R1U','NIM_B7L1L','NIM_B7L1U',
        'NIM_A7R1L','NIM_A7R1U','NIM_A7L1L','NIM_A7L1U',
        ### For L1_ALFA
        'NIM_ALFA_LOG',
        'NIM_B7R1L_OD','NIM_B7R1U_OD','NIM_B7L1L_OD','NIM_B7L1U_OD',
        'NIM_A7R1L_OD','NIM_A7R1U_OD','NIM_A7L1L_OD','NIM_A7L1U_OD',
        ### For L1_ALFA
        'NIM_ALFA_LOG_OD',
        ])
    StoredValue   = []

class RemapThresholdsAsListed(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

class CtpIdMap(JobProperty):
    statusOn = True
    allowedTypes = ['dict']
    StoredValue = {}

class CustomThresholdMap(JobProperty):
    statusOn = True
    allowedTypes = ['dict']
    StoredValue = {}
    
class ThresholdMap(JobProperty):
    statusOn = True
    allowedTypes = ['dict']
    StoredValue = {}

class ItemMap(JobProperty):
    statusOn = True
    allowedTypes = ['dict']
    StoredValue = {}

class MuonHIV1Menus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [ 
        'InitialBeam_HI_v1', 'InitialBeam_HI_v1_no_prescale', \
        'MC_InitialBeam_HI_v1', 'MC_InitialBeam_HI_v1_no_prescale', \
        'Physics_HI_v1', 'Physics_HI_v1_no_prescale', \
        'MC_HI_v1', 'MC_HI_v1_no_prescale', 'MC_HI_v1_pPb_mc_prescale',\
	]

class MuonHIMenus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [ 
	'Physics_HI_v2', 'Physics_HI_v2_no_prescale',\
        'MC_HI_v2', 'MC_HI_v2_pPb_mc_prescale', 'MC_HI_v2_no_prescale',\
	]

class NoTRTMuonMenus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [ 
        'InitialBeam_HI_v1', 'InitialBeam_HI_v1_no_prescale', \
        'MC_InitialBeam_HI_v1', 'MC_InitialBeam_HI_v1_no_prescale', \
        'Physics_HI_v1', 'Physics_HI_v1_no_prescale', \
        'MC_HI_v1', 'MC_HI_v1_no_prescale', 'MC_HI_v1_pPb_mc_prescale',\
        'Physics_HI_v2', 'Physics_HI_v2_no_prescale',\
        'MC_HI_v2', 'MC_HI_v2_pPb_mc_prescale', 'MC_HI_v2_no_prescale',\
	]
    
class NoVHThresholdMenus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [ 
        'Physics_pp_v2', 'Physics_pp_v2_cosmics_prescale', \
        'MC_pp_v2', 'MC_pp_v2_no_prescale', 'MC_pp_v2_loose_mc_prescale', 'MC_pp_v2_tight_mc_prescale', \
        'Physics_HI_v1', 'Physics_HI_v1_no_prescale', \
        'MC_HI_v1', 'MC_HI_v1_no_prescale', 'MC_HI_v1_pPb_mc_prescale',\
        'Physics_HI_v2',  'Physics_HI_v2_no_prescale',\
        'MC_HI_v2', 'MC_HI_v2_pPb_mc_prescale', 'MC_HI_v2_no_prescale',\
	]

class VHThresholdMenus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [ 
        'Physics_pp_v3', 'Physics_pp_v3_cosmics_prescale', \
        'MC_pp_v3', 'MC_pp_v3_no_prescale', 'MC_pp_v3_loose_mc_prescale', 'MC_pp_v3_tight_mc_prescale', \
        'Physics_pp_v4', 'Physics_pp_v4_cosmics_prescale', \
        'MC_pp_v4', 'MC_pp_v4_no_prescale', 'MC_pp_v4_loose_mc_prescale', 'MC_pp_v4_tight_mc_prescale', 'MC_pp_v4_upgrade_mc_prescale', \
	]
    
class MU0ThresholdMenus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [ 
        'Physics_pp_v2', 'Physics_pp_v2_cosmics_prescale', \
        'MC_pp_v2', 'MC_pp_v2_no_prescale', 'MC_pp_v2_loose_mc_prescale', 'MC_pp_v2_tight_mc_prescale', \
        'Physics_HI_v1', 'Physics_HI_v1_no_prescale', \
        'MC_HI_v1', 'MC_HI_v1_no_prescale', 'MC_HI_v1_pPb_mc_prescale',\
        'Physics_HI_v2', 'Physics_HI_v2_no_prescale',\
        'MC_HI_v2', 'MC_HI_v2_pPb_mc_prescale', 'MC_HI_v2_no_prescale',\
	]

class MU4ThresholdMenus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [ 
        'Physics_pp_v3', 'Physics_pp_v3_cosmics_prescale', \
        'MC_pp_v3', 'MC_pp_v3_no_prescale', 'MC_pp_v3_loose_mc_prescale', 'MC_pp_v3_tight_mc_prescale', \
        'Physics_pp_v4', 'Physics_pp_v4_cosmics_prescale', \
        'MC_pp_v4', 'MC_pp_v4_no_prescale', 'MC_pp_v4_loose_mc_prescale', 'MC_pp_v4_tight_mc_prescale', 'MC_pp_v4_upgrade_mc_prescale', \
	]

class HIv1Menus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [ 
        'Physics_HI_v1', 'Physics_HI_v1_no_prescale', \
        'MC_HI_v1', 'MC_HI_v1_no_prescale', 'MC_HI_v1_pPb_mc_prescale',\
        ]

class HIv2Menus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [ 
        'Physics_HI_v2', 'Physics_HI_v2_no_prescale',\
        'MC_HI_v2_pPb_mc_prescale', 'MC_HI_v2_no_prescale', 'MC_HI_v2',\
        ]

class PPv2Menus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [ 
        'Physics_pp_v2', 'Physics_pp_v2_cosmics_prescale', \
        'MC_pp_v2', 'MC_pp_v2_no_prescale', 'MC_pp_v2_loose_mc_prescale', 'MC_pp_v2_tight_mc_prescale', \
	]    

class PPv3Menus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [ 
        'Physics_pp_v3', 'Physics_pp_v3_cosmics_prescale', \
        'MC_pp_v3', 'MC_pp_v3_no_prescale', 'MC_pp_v3_loose_mc_prescale', 'MC_pp_v3_tight_mc_prescale', \
	]    

class PPv4Menus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [
        'Physics_pp_v4', 'Physics_pp_v4_cosmics_prescale', \
        'MC_pp_v4', 'MC_pp_v4_no_prescale', 'MC_pp_v4_loose_mc_prescale', 'MC_pp_v4_tight_mc_prescale', 'MC_pp_v4_upgrade_mc_prescale', \
        ]

class ALFAv2Menus(JobProperty):
    statusOn = True
    allowedTypes = ['list']
    StoredValue = [
        'L1_alfa_v2', \
        ]
                

    
_flags.append(items)
_flags.append(thresholds)
_flags.append(RemapThresholdsAsListed)
_flags.append(CtpIdMap)
_flags.append(ThresholdMap)
_flags.append(ItemMap)
_flags.append(CustomThresholdMap)
_flags.append(NoTRTMuonMenus)
_flags.append(MuonHIV1Menus)
_flags.append(MuonHIMenus)
_flags.append(NoVHThresholdMenus)
_flags.append(VHThresholdMenus)
_flags.append(MU0ThresholdMenus)
_flags.append(MU4ThresholdMenus)
_flags.append(HIv1Menus)
_flags.append(HIv2Menus)
_flags.append(PPv2Menus)
_flags.append(PPv3Menus)
_flags.append(PPv4Menus)
_flags.append(ALFAv2Menus)
class Lvl1(JobPropertyContainer):
    """ Lvl1 flags used in menu generation """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(Lvl1)

for flag in _flags:
    TriggerFlags.Lvl1.add_JobProperty(flag)
del _flags

# make an alias
Lvl1Flags = TriggerFlags.Lvl1

# Strictly speaking not L1...
class physics_pass_through(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False
class zero_stream_prescales(JobProperty):
    statusOn = True
    allowedTypes = ['bool']
    StoredValue = False

TriggerFlags.add_JobProperty(physics_pass_through)
TriggerFlags.add_JobProperty(zero_stream_prescales)


#  LocalWords:  AllowedList


            
##########################################################################################
#
# Add new entry to ItemMap
#
def addToItemMap(key, val):
    """
    Add new entry to ItemMap if key does not already exists
    """

    if key in Lvl1Flags.ItemMap():
        log.warning('Key already exists in ItemMap: '+key)
    else:
        # Add new entry
        Lvl1Flags.ItemMap()[key] = val
        
##########################################################################################
#
# Add new entry to ThresholdMap
#
def addToThresholdMap(key, val):
    """
    Add new entry to ThresholdMap if key does not already exists
    """

    if key in Lvl1Flags.ThresholdMap():
        log.warning('Key already exists in ThresholdMap: '+key)
        return
    else:
        # Add new entry
        Lvl1Flags.ThresholdMap()[key] = val
