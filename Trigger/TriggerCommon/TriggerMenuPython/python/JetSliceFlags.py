# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Egamma slice specific flags  """

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper, AllowedList

__author__  = 'T. Bold'
__version__="$Revision: 1.31 $"
__doc__="Jet slice specific flags  "


_flags = [] 
class signatures(JobProperty):
    """ signatures in Jet slice """
    statusOn=True
    allowedTypes=['list']
    StoredValue   = ["j5","j10","j20","j23","j35","j42","j70","j120","j200",
                     "3j10","4j10",
                     '4j35_3j45_2j50_j60', '5j10_4j35_3j45_j60', 
                     "5j20","6j20",
                     "fj18","fj35","fj70",
                     #"fj120",
                    "2fj70",
                     "2fj18","2fj35",
                     "je60","je100",
                     "je120","je220","je280","je340",
                     'j35L1J18', 'j45L1J23', 'j60L1J42', 
                     "j35_L1TAU_HV", "j35_Trackless_HV",
                     "2j10_deta3_5", "2j20_deta3_5", "2j40_deta3_5", "2j10_deta5","2j20_deta5","2j40_deta4",
                     "j10_noise","j10_c4",
                     "j20_noise","j20_c4",
                     "j15_a2hi_EFFS",
                     "j20_a2hi_EFFS",                     
                     "j20_a4hi_EFFS",                     
                     "j15_a2tc_EFFS",
                     "j20_a2tc_EFFS",
                     '6j30_a4tcem_L2a4cc',
                     'j10_u0uchad_LArNoiseBurst', 'L1J5_NoAlg',
                     'j10_u0uchad_firstempty_LArNoiseBurst',
                     'j10_u0uchad_empty_LArNoiseBurst',                           
                     'j100_LArNoiseBurst',
                     'j165_LArNoiseBurstT',
                     'j130_LArNoiseBurstT',
                     'j100_LArNoiseBurstT',
                     'j30_LArNoiseBurst_empty',
                     'j55_LArNoiseBurst_firstempty',
                     'j55_LArNoiseBurst_firstemptyT',
                     'j55_LArNoiseBurst_empty',
                     #for testing
                     'j30_a4tcem',
                     'j180_a10tcem',
                     'j110_j45_a4tchad',
                     #dummy
                     'roi_dummy_J15','roi_dummy_4J15','roi_dummy_4J15',
                     ##
                     '4j80_a4tchad_L2FShad',
                     '4j80_a4tchad_L2FShadPS',
                     '5j55_a4tchad_L2FShadPS',
                     '6j45_a4tchad_L2FShadPS',
                     'fj55_a4tchad_L2FShad',
                     'L2_j10000_a4cchad_L1RD0',
                     ]
_flags.append(signatures)



# create container
from TriggerMenuPython.CommonSliceHelper import CommonSliceHelper

class JetSlice(JobPropertyContainer, CommonSliceHelper):
    """ Jet Slice Flags """

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.add_Container(JetSlice)

# add add common slice flags
TriggerFlags.JetSlice.import_JobProperties('TriggerMenuPython.CommonSliceFlags')

for flag in _flags:
    TriggerFlags.JetSlice.add_JobProperty(flag)
del _flags

# make an alias
JetSliceFlags = TriggerFlags.JetSlice

