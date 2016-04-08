# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" MinBias slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'T.Bold'
__version__="$Revision: 1.18 $"
__doc__="Heavy Ion slice specific flags  "

#
# This file should not be modified without consulting 
# the Heavy Ion trigger slice coordinator.
#
# The Minimum Bias trigger slice is documented at:
# https://twiki.cern.ch/twiki/bin/view/Atlas/MinbiasTrigger
#

#===================================================================

_flags = [] 

class signatures(JobProperty):
    """ signatures in MinBias slice """
    statusOn=True
    allowedTypes=['list']
    allowedValues = AllowedList(['pass_all', # chain passing all events, seeded by OR of all L1 (can be prescaled), 
                                 'sp_nocut',  # space points counting for without any hypothesis in the sequence
                                 'prompt',
                                 'j15_Ecorr',
                                 'j20_Ecorr',
                                 'fj20_Ecorr',
                                 #UPC
                                 'L1ZDC_VTE50_upc',
                                 'L1MU0_VTE50_upc',
                                 'L1MU0_MV_VTE50_upc',
                                 'L1MU4_VTE50_upc',
                                 'L1MU4_MV_VTE50_upc',
                                 'L1EM3_VTE50_upc',
                                 'L1EM3_MV_VTE50_upc',
                                 'L1ZDC_VTE50_mv_loose2_upc',
                                 'L1ZDC_OR_mv_loose2_upc',
                                 'L1ZDC_VTE50_mv_medium2_upc',
                                 'L1ZDC_OR_mv_medium2_upc',
                                 #p+Pb UPC
				 'L1MU0_VTE20_hip_loose2_upc',
                                 'L1EM3_VTE20_hip_loose2_upc',
                                 'L1EM3_VTE50_hip_loose2_upc',
                                 'L1ZDC_A_VTE20_hip_loose2_upc',
                                 'L1ZDC_C_VTE20_hip_loose2_upc',
                                 #backup UPC
                                 'L1ZDC_VTE50_loose_upc',
                                 'L1MU0_VTE50_loose_upc',
                                 'L1MU0_MV_VTE50_loose_upc',
                                 'L1MU4_VTE50_loose_upc',
                                 'L1MU4_MV_VTE50_loose_upc',
                                 'L1EM3_VTE50_loose_upc',
                                 'L1EM3_MV_VTE50_loose_upc',
                                 # peripheral events
                                 'fte75_veto',
                                 'fte75_veto_mbMbts_2_2',
				 'centrality100_60',
				 'centrality80_60',
                                 #
                                 'mbZdc_a_c_L1VTE50_trk',
                                 'mbMbts_1_1_L1VTE50_trk',
                                 'mbMbts_2_2_L1VTE50_trk',
                                 #met for p+Pb
				 'xe20_FEB_L1TE65',
                                 'xe20_FEB_L1TE90',
				 'xe25_FEB_L1TE65',
                                 'xe25_FEB_L1TE90',
                                 'xe20_FEB_L1EM5',
                                 'xe20_FEB_L1J10', 
                                 'xe20_FEB_L1MU0',
                                 'xe30_FEB_L1TE65',
                                 'xe30_FEB_L1TE90',
                                 'xe30_FEB_L1EM5', 
                                 'xe30_FEB_L1J10', 
                                 'xe30_FEB_L1MU0',
                                 'xe40_FEB_L1TE65',
                                 'xe40_FEB_L1TE90',
                                 'xe40_FEB_L1EM5',
                                 'xe40_FEB_L1J10',
                                 'xe40_FEB_L1MU0',
                                 'zerobias_Overlay_L2te',
                                 ])
    
    StoredValue   = None
    

_flags.append(signatures)

# create container
class HeavyIonSlice(JobPropertyContainer, CommonSliceHelper):
    """ HeavyIon Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(HeavyIonSlice)

# add add common slice flags
TriggerFlags.HeavyIonSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.HeavyIonSlice.add_JobProperty(flag)
del _flags

# make an alias
HeavyIonSliceFlags = TriggerFlags.HeavyIonSlice

